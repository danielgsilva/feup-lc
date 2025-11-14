#ifndef __MOUSE_H
#define __MOUSE_H

#include "i8042.h"
#include "kbc.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

/*
// lab4.h
struct packet {
  uint8_t bytes[3]; // mouse packet raw bytes
  bool rb, mb, lb;  // right, middle and left mouse buttons pressed
  int16_t delta_x;  // mouse x-displacement: rightwards is positive
  int16_t delta_y;  // mouse y-displacement: upwards is positive
  bool x_ov, y_ov;  // mouse x-displacement and y-displacement overflows
};
*/

typedef struct {
  int16_t x;
  int16_t y;
  bool mouse_mv;
} MouseState;

int(mouse_subscribe_int)(uint8_t *bit_no);

int(mouse_unsubscribe_int)();

void(mouse_ih)(void);

void(mouse_parse_packet)(struct packet *pp);

/**
 * @brief Enables/disables stream mode data reporting,
 * by sending the respective command to the mouse.
 * This function is NOT provided by the LCF.
 * @param enable If true enable, otherwise disable
 * @return Returns O on success, and non-zero otherwise
 */
int(mouse_data_reporting)(bool enable);

#endif
