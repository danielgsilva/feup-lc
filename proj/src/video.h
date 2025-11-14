#ifndef __VIDEO_H
#define __VIDEO_H

#include "sprite.h"
#include <lcom/lcf.h>
#include <stdint.h>

#define FRAME_RATE 30
#define DIRECT_COLOR_MODE_8_8_8 0x115 // 800x600

int(set_vbe_mode)(uint16_t mode);
void *(vg_init) (uint16_t mode);
int(vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
void(vg_fill)(uint32_t color);
void(vg_double_buffering)();

#endif
