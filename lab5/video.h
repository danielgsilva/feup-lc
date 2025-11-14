#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#define DIRECT_COLOR_MODE 0x6
#define INDEXED_COLOR_MODE 0x105

int(set_vbe_mode)(uint16_t mode);
void *(vg_init) (uint16_t mode);
int(vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int(vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
uint32_t(R)(uint32_t color);
uint32_t(G)(uint32_t color);
uint32_t(B)(uint32_t color);
int(vg_draw_pixmap)(xpm_map_t xpm, uint16_t x, uint16_t y);
void(vg_fill)(uint32_t color);
void(vg_double_buffering)();

#endif
