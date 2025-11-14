#ifndef __MOUSE_H
#define __MOUSE_H

#include "i8042.h"
#include "kbc.h"
#include <stdbool.h>
#include <stdint.h>

int(mouse_subscribe_int)(uint8_t *bit_no);

int(mouse_unsubscribe_int)();

void(mouse_ih)(void);

void(mouse_parse_packet)(struct packet *pp);

int(mouse_data_reporting)(bool enable);

#endif
