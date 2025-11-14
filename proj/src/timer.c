#include "i8254.h"
#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>

static int timer0_hook_id;
unsigned long timer0_counter = 0;

int(timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  timer0_hook_id = TIMER0_IRQ;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer0_hook_id) != OK) {
    printf("error: sys_irqsetpolicy()\n");
    return 1;
  }
  *bit_no = TIMER0_IRQ;

  return (timer0_hook_id == TIMER0_IRQ);
}

int(timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer0_hook_id) != OK) {
    printf("error: sys_irqrmpolicy()\n");
    return 1;
  }

  return 0;
}

void(timer_int_handler)() {
  timer0_counter++;
}
