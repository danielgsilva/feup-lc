#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042: The keyboard controller (KBC).
 */

#define DELAY_US 20000

#define BREAK_CODE BIT(7)      // The break code of a key differs from the make code of that key on the MSB
#define TWO_BYTE_SCANCODE 0xe0 // Two-byte long scancodes usually use 0xE0 as their first byte
#define ESC_BREAK_CODE 0x81

#define KBC_IRQ 1
#define KBC_STAT_REG 0x64
#define KBC_PARITY BIT(7)
#define KBC_TIMEOUT BIT(6)
#define KBC_AUX BIT(5) // mouse data
#define KBC_IBF BIT(1)
#define KBC_OBF BIT(0)
#define KBC_IN_BUF_CMD 0x64
#define KBC_IN_BUF_CMD_ARGS 0x60
#define KBC_OUT_BUF 0x60

#define KBC_CMD_READ_CMD_BYTE 0x20
#define KBC_CMD_WRITE_CMD_BYTE 0x60
#define KBC_CMD_BYTE_INT BIT(0) // enable interrupt on OBF, from keyboard
#define KBC_CMD_BYTE_INT2 BIT(1) // enable interrupt on OBF, from mouse

/**@}*/

#endif /* _LCOM_I8042_H */
