#ifndef __KBC_H
#define __KBC_H

#include "i8042.h"
#include <stdbool.h>
#include <stdint.h>
#include <lcom/lcf.h>

/**
 * @brief Lê o status register.
 * @param st endereço de memória que será preenchido com o valor do registo
*/
int(kbc_get_status)(uint8_t *st);

/**
 * @brief Lê o output buffer.
 * Caso haja algum erro na comunicação ou dados inesperados, descarta esse valor.
 * @param kbc_aux If kbc_aux = 0 => data in the OUT_BUF is coming from the keyboard
 * Else if kbc_aux = 1 => data in the OUT_BUF is coming from the mouse
*/
int(kbc_read_out_buf)(uint8_t *data, bool kbc_aux);


/**
 * @brief Envia um comando ao KBC.
 * @param port porto do KBC
 * @param cmd comando
*/
int(kbc_write_command)(int port, uint8_t cmd);

#endif
