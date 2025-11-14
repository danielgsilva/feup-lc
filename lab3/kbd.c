#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"
#include "kbd.h"

static int kbd_hook_id;
uint8_t scancode;

int(kbd_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  kbd_hook_id = KBC_IRQ;
  if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK) {
    printf("error: sys_irqsetpolicy()\n");
    return 1;
  }
  *bit_no = KBC_IRQ;

  return (kbd_hook_id == KBC_IRQ);
}

int(kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&kbd_hook_id) != OK) {
    printf("error: sys_irqrmpolicy()\n");
    return 1;
  }

  return 0;
}

void(kbc_ih)() {
  if (kbc_read_out_buf(&scancode) != 0)
    printf("error: kbc_read_out_buf()\n");
}

int(kbc_get_status)(uint8_t *st) {
  if (st == NULL)
    return 1;
  return util_sys_inb(KBC_STAT_REG, st);
}

int(kbc_read_out_buf)(uint8_t *data) {
  if (data == NULL)
    return 1;
  uint8_t tmp_data, st, attempts = 3;
  while (attempts) {
    if (kbc_get_status(&st) != 0) {
      printf("error: kbc_get_status()\n");
      return 1;
    }
    /* loop while 8042 output buffer is empty */
    if (st & KBC_OBF) {
      if (util_sys_inb(KBC_OUT_BUF, &tmp_data) != 0)
        return 1;
      if (st & (KBC_PARITY | KBC_TIMEOUT)) {
        printf("error in the (serial) communication\n");
        return 1;
      }
      else {
        *data = tmp_data;
        return 0;
      }
    }
    tickdelay(micros_to_ticks(DELAY_US));
    attempts--;
  }
  printf("error: attempts exceeded\n");
  return 1;
}

int(kbc_write_command)(int port, uint8_t cmd) {
  uint8_t st, attempts = 3;
  while (attempts) {
    if (kbc_get_status(&st) != 0) {
      printf("error: kbc_get_status()\n");
      return 1;
    }
    /* loop while 8042 input buffer is not empty */
    if (!(st & KBC_IBF)) {
      if (sys_outb(port, cmd) != 0) {
        printf("error: sys_outb()\n");
        return 1;
      }
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
    attempts--;
  }
  printf("error: attempts exceeded\n");
  return 1;
}

int(kbc_read_cmd_byte)(uint8_t *cmd_byte) {
  if (cmd_byte == NULL)
    return 1;
  if (kbc_write_command(KBC_IN_BUF_CMD, KBC_CMD_READ_CMD_BYTE) != 0) {
    printf("error: kbc_write_cmd()\n");
    return 1;
  }
  if (kbc_read_out_buf(cmd_byte) != 0) {
    printf("error: kbc_read_out_buf()\n");
    return 1;
  }

  return 0;
}

int(kbc_write_cmd_byte)(uint8_t cmd_byte) {
  if (kbc_write_command(KBC_IN_BUF_CMD, KBC_CMD_WRITE_CMD_BYTE) != 0) {
    printf("error: kbc_write_cmd()\n");
    return 1;
  }
  if (kbc_write_command(KBC_IN_BUF_CMD_ARGS, cmd_byte) != 0) {
    printf("error: kbc_write_cmd()\n");
    return 1;
  }

  return 0;
}
