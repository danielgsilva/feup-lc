#ifndef __KBD_H
#define __KBD_H

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"

int(kbd_subscribe_int)(uint8_t *bit_no);

int(kbd_unsubscribe_int)();

void(kbc_ih)();

int(kbc_get_status)(uint8_t *st);

int(kbc_read_out_buf)(uint8_t *data);

int(kbc_write_command)(int port, uint8_t cmd);

int(kbc_read_cmd_byte)(uint8_t *cmd_byte);

int(kbc_write_cmd_byte)(uint8_t cmd_byte);


#endif
