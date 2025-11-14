#include "sprite.h"

/** Creates a new sprite from XPM "xpm", with specified
 * position (within the screen limits);
 * Does not draw the sprite on the screen
 * Returns NULL on invalid pixmap.
 */
Sprite *create_sprite(xpm_map_t xpm, int x, int y) {
  // allocate space for the "object"
  Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
  if (sp == NULL)
    return NULL;
  // read the sprite pixmap
  xpm_image_t img;                                         // pixmap and metadata
  sp->map = (uint32_t *) xpm_load(xpm, XPM_8_8_8_8, &img); // get the pixmap from the XPM
  if (sp->map == NULL) {
    free(sp);
    return NULL;
  }
  sp->width = img.width;
  sp->height = img.height;
  sp->x = x;
  sp->y = y;
  return sp;
}

/**
 * The "sp" sprite is erased from memory
 * and used resources released.
*/
void destroy_sprite(Sprite *sp) {
  if (sp == NULL)
    return;
  if (sp->map)
    free(sp->map);
  free(sp);
  sp = NULL;
}

int(draw_sprite)(Sprite *sp) {
  // copy it to graphics memory
  uint32_t color;
  for (int row = 0; row < sp->height; row++) {
    for (int col = 0; col < sp->width; col++) {
      color = sp->map[col + row * sp->width];
      if (color != TRANSPARENT_COLOR) {
        if (vg_draw_pixel(col + sp->x, row + sp->y, color))
          return 1;
      }
    }
  }
  return 0;
}
