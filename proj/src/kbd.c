#include "kbd.h"

static int kbd_hook_id;
uint8_t scancode;

int(kbd_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  kbd_hook_id = KBD_IRQ;
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK) {
    printf("error: sys_irqsetpolicy()\n");
    return 1;
  }
  *bit_no = KBD_IRQ;

  return (kbd_hook_id == KBD_IRQ);
}

int(kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&kbd_hook_id) != OK) {
    printf("error: sys_irqrmpolicy()\n");
    return 1;
  }

  return 0;
}

void(kbd_ih)() {
  if (kbc_read_out_buf(&scancode, 0) != 0)
    printf("error: kbc_read_out_buf()\n");
}
