#ifndef __KBD_H
#define __KBD_H

#include "kbc.h"
#include <lcom/lcf.h>

int(kbd_subscribe_int)(uint8_t *bit_no);

int(kbd_unsubscribe_int)();

void(kbd_ih)();

#endif
