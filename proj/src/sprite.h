#ifndef __SPRITE_H
#define __SPRITE_H

#include <lcom/lcf.h>
#include "video.h"
#include <stdint.h>

typedef struct {
  int x, y;          // current position
  int width, height; // dimensions
  // int xspeed, yspeed; // current speed
  uint32_t *map; // the pixmap
} Sprite;

#define BACKGROUND_COLOR 0x721919
#define TRANSPARENT_COLOR 0x000001 // the pixmap uses this unused color for the background, which is assumed to be transparent

Sprite *create_sprite(xpm_map_t xpm, int x, int y);
void destroy_sprite(Sprite *sp);
int draw_sprite(Sprite *sp);

#endif
