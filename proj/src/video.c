#include "video.h"

vbe_mode_info_t vmi_p;
static uint8_t *video_mem; /* Process (virtual) address to which VRAM is mapped */
static uint8_t *video_second_buffer;
static uint16_t h_res;           /* Horizontal resolution in pixels */
static uint16_t v_res;           /* Vertical resolution in pixels */
static unsigned bits_per_pixel;  /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */

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
  bytes_per_pixel = ((vmi_p.BitsPerPixel + 7) / 8);
  h_res = vmi_p.XResolution;
  v_res = vmi_p.YResolution;
  vram_base = vmi_p.PhysBasePtr;
  vram_size = v_res * h_res * bytes_per_pixel;

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
  if (x >= h_res || y >= v_res || x < 0 || y < 0)
    return 1;
  // using pointer arithmetic
  /*uint8_t *ptr = (uint8_t *) video_mem + h_res * y * bytes_per_pixel + x * bytes_per_pixel;
  for (unsigned int i = 0; i < bytes_per_pixel; i++) {
    uint8_t tmp_color = (color & 0xff);
    *(ptr + i) = tmp_color;
    color >>= 8;
  }*/
  memcpy(video_second_buffer + (h_res * y * bytes_per_pixel) + (x * bytes_per_pixel), &color, bytes_per_pixel);
  return 0;
}

void(vg_fill)(uint32_t color) {
  // memset(video_second_buffer, color, h_res * v_res * bytes_per_pixel);
  for (int row = 0; row < v_res; row++) {
    for (int col = 0; col < h_res; col++) {
      if (vg_draw_pixel(col, row, color))
        return;
    }
  }
}

void(vg_double_buffering)() {
  memcpy(video_mem, video_second_buffer, h_res * v_res * bytes_per_pixel);
}
