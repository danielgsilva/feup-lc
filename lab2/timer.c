#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id;
unsigned int counter = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (freq > TIMER_FREQ || freq < 19) { // TIMER_FREQ/2^16 (16-bit counter)
    printf("error: freq out of range\n");
    return 1;
  }
  uint8_t st;
  if (timer_get_conf(timer, &st) != 0) {
    printf("error: timer_get_conf()\n");
    return 1;
  }
  uint8_t ctrl_word = 0;
  ctrl_word = (TIMER_LSB_MSB | (st & 0x0f));
  if (timer == 0)
    ctrl_word |= TIMER_SEL0;
  else if (timer == 1)
    ctrl_word |= TIMER_SEL1;
  else if (timer == 2)
    ctrl_word |= TIMER_SEL2;

  if (sys_outb(TIMER_CTRL, ctrl_word) != 0) {
    printf("error: sys_outb()\n");
    return 1;
  }
  uint16_t init_counting_val = TIMER_FREQ / freq;
  uint8_t lsb, msb;
  if (util_get_LSB(init_counting_val, &lsb) != 0) {
    printf("error: util_get_LSB()\n");
    return 1;
  }
  if (sys_outb(TIMER_0 + timer, lsb) != 0) {
    printf("error: sys_outb()\n");
    return 1;
  }
  if (util_get_MSB(init_counting_val, &msb) != 0) {
    printf("error: util_get_MSB()\n");
    return 1;
  }
  if (sys_outb(TIMER_0 + timer, msb) != 0) {
    printf("error: sys_outb()\n");
    return 1;
  }

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  hook_id = TIMER0_IRQ;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) {
    printf("error: sys_irqsetpolicy()\n");
    return 1;
  }
  *bit_no = TIMER0_IRQ;

  return (hook_id == TIMER0_IRQ);
}

int(timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    printf("error: sys_irqrmpolicy()\n");
    return 1;
  }

  return 0;
}

void(timer_int_handler)() {
  counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL || timer < 0 || timer > 2)
    return 1;
  uint8_t read_back_cmd = 0;
  read_back_cmd = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
  if (sys_outb(TIMER_CTRL, read_back_cmd) != 0) {
    printf("error: sys_outb()\n");
    return 1;
  }
  if (util_sys_inb(TIMER_0 + timer, st) != 0) {
    printf("error: sys_inb()\n");
    return 1;
  }

  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val val;
  if (field == tsf_base) {
    val.bcd = st & TIMER_BCD;
  }
  else if (field == tsf_mode) {
    uint8_t count_mode = (st & (BIT(3) | BIT(2) | BIT(1)));
    if (count_mode == TIMER_MODE_5)
      val.count_mode = 5;
    else if (count_mode == TIMER_MODE_4)
      val.count_mode = 4;
    else if (count_mode == TIMER_SQR_WAVE || count_mode == TIMER_SQR_WAVE_INTEL)
      val.count_mode = 3;
    else if (count_mode == TIMER_RATE_GEN || count_mode == TIMER_RATE_GEN_INTEL)
      val.count_mode = 2;
    else if (count_mode == TIMER_HW_RETRIGGERABLE)
      val.count_mode = 1;
    else if (count_mode == TIMER_INT_TERMINAL_CNT)
      val.count_mode = 0;
  }
  else if (field == tsf_initial) {
    uint8_t in_mode = (st & (BIT(5) | BIT(4)));
    if (in_mode == TIMER_LSB_MSB)
      val.in_mode = MSB_after_LSB;
    else if (in_mode == TIMER_MSB)
      val.in_mode = MSB_only;
    else if (in_mode == TIMER_LSB)
      val.in_mode = LSB_only;
    else
      val.in_mode = INVAL_val;
  }
  else if (field == tsf_all)
    val.byte = st;

  if (timer_print_config(timer, field, val) != 0) {
    printf("error: timer_print_config()\n");
    return 1;
  }

  return 0;
}
