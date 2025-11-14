#ifndef __KBC_H
#define __KBC_H

#include "i8042.h"
#include <stdbool.h>
#include <stdint.h>

int(kbc_get_status)(uint8_t *st);

/*
kbc_aux = 0 => data in the OUT_BUF is coming from the keyboard
kbc_aux = 1 => data in the OUT_BUF is coming from the mouse
*/
int(kbc_read_out_buf)(uint8_t *data, bool kbc_aux);

int(kbc_write_command)(int port, uint8_t cmd);

int(kbc_read_cmd_byte)(uint8_t *cmd_byte);

int(kbc_write_cmd_byte)(uint8_t cmd_byte);

#endif
