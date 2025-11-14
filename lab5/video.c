#include "video.h"
#include <lcom/lcf.h>

static vbe_mode_info_t vmi_p;
static uint8_t *video_mem; /* Process (virtual) address to which VRAM is mapped */
static uint8_t *video_second_buffer;
static uint16_t h_res;          /* Horizontal resolution in pixels */
static uint16_t v_res;          /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int(set_vbe_mode)(uint16_t mode) {
  reg86_t r86;
  /* Specify the appropriate register values */
  memset(&r86, 0, sizeof(r86)); // zero the structure
  r86.intno = 0x10;             // BIOS video services
  r86.ax = 0x4f02;              // VBE call, function 02 -- set VBE mode
  r86.bx = BIT(14) | mode;      // set bit 14: linear framebuffer

  /* Make the BIOS call */
  if (sys_int86(&r86) != OK) {
    printf("\tset_vbe_mode(): sys_int86() failed \n");
    return 1;
  }

  return 0;
}

void *(vg_init) (uint16_t mode) {
  /* map the video memory to the process' address space */
  struct minix_mem_range mr;
  unsigned int vram_base; /* VRAM's physical addresss */
  unsigned int vram_size; /* VRAM's size, but you can use the frame-buffer size, instead */
  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  memset(&vmi_p, 0, sizeof(vmi_p)); // zero the structure
  if (vbe_get_mode_info(mode, &vmi_p))
    return NULL;
  bits_per_pixel = vmi_p.BitsPerPixel;
  h_res = vmi_p.XResolution;
  v_res = vmi_p.YResolution;
  vram_base = vmi_p.PhysBasePtr;
  vram_size = v_res * h_res * ((bits_per_pixel + 7) / 8);

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return NULL;
  }

  video_second_buffer = (uint8_t *) malloc(vram_size);

  /* change the video mode to that in its argument */
  if (set_vbe_mode(mode))
    return NULL;

  return video_mem;
}

int(vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if (x > h_res || y > v_res)
    return 1;
  unsigned bytes_per_pixel = (bits_per_pixel + 7) / 8;
  // using pointer arithmetic
  /*uint8_t *ptr = (uint8_t *) video_mem + h_res * y * bytes_per_pixel + x * bytes_per_pixel;
  for (unsigned int i = 0; i < bytes_per_pixel; i++) {
    uint8_t tmp_color = color & 0xff;
    *(ptr + i) = tmp_color;
    color >>= 8;
  }*/
  memcpy(video_second_buffer + h_res * y * bytes_per_pixel + x * bytes_per_pixel, &color, bytes_per_pixel);
  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (int i = 0; i < len; i++) {
    if (vg_draw_pixel(x + i, y, color))
      return 1;
  }
  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (int i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color))
      return 1;
  }
  return 0;
}

int(vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  uint16_t width = (h_res / no_rectangles);
  uint16_t height = (v_res / no_rectangles);
  uint32_t color = 0;
  for (int row = 0; row < no_rectangles; row++) {
    for (int col = 0; col < no_rectangles; col++) {
      if (vmi_p.MemoryModel == DIRECT_COLOR_MODE) {
        uint32_t red, green, blue;
        red = ((R(first) + col * step) % (1 << vmi_p.RedMaskSize));
        green = ((G(first) + row * step) % (1 << vmi_p.GreenMaskSize));
        blue = ((B(first) + (col + row) * step) % (1 << vmi_p.BlueMaskSize));
        color = (red << vmi_p.RedFieldPosition | green << vmi_p.GreenFieldPosition | blue << vmi_p.BlueFieldPosition);
      }
      else
        color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);

      if (vg_draw_rectangle(col * width, row * height, width, height, color))
        return 1;
    }
  }
  return 0;
}

uint32_t(R)(uint32_t color) {
  return ((color >> vmi_p.RedFieldPosition) & (BIT(vmi_p.RedMaskSize) - 1));
}

uint32_t(G)(uint32_t color) {
  return ((color >> vmi_p.GreenFieldPosition) & (BIT(vmi_p.GreenMaskSize) - 1));
}

uint32_t(B)(uint32_t color) {
  return ((color >> vmi_p.BlueFieldPosition) & (BIT(vmi_p.BlueMaskSize) - 1));
}

int(vg_draw_pixmap)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;                 // pixmap and metadata
  uint8_t *map;                    // pixmap itself
  map = xpm_load(xpm, type, &img); // get the pixmap from the XPM
  if (map == NULL)
    return 1;

  // copy it to graphics memory
  for (int row = 0; row < img.height; row++) {
    for (int col = 0; col < img.width; col++) {
      if (vg_draw_pixel(col + x, row + y, *map))
        return 1;
      map++;
    }
  }

  return 0;
}

void(vg_fill)(uint32_t color) {
  unsigned bytes_per_pixel = (bits_per_pixel + 7) / 8;
  memset(video_second_buffer, color, h_res * v_res * bytes_per_pixel);
}

void(vg_double_buffering)() {
  unsigned bytes_per_pixel = (bits_per_pixel + 7) / 8;
  memcpy(video_mem, video_second_buffer, h_res * v_res * bytes_per_pixel);
}
