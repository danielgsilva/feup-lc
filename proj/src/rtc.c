#include "rtc.h"

static int rtc_hook_id;
RTC_Time rtc_time;

int(rtc_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  rtc_hook_id = RTC_IRQ;
  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hook_id) != OK) {
    printf("error: sys_irqsetpolicy()\n");
    return 1;
  }
  *bit_no = RTC_IRQ;

  return (rtc_hook_id == RTC_IRQ);
}

int(rtc_unsubscribe_int)() {
  if (sys_irqrmpolicy(&rtc_hook_id) != OK) {
    printf("error: sys_irqrmpolicy()\n");
    return 1;
  }

  return 0;
}

int(rtc_read)(uint8_t reg, uint8_t *data) {
  if (data == NULL)
    return 1;
  if (sys_outb(RTC_ADDR_REG, reg))
    return 1;
  if (util_sys_inb(RTC_DATA_REG, data))
    return 1;
  return 0;
}

int(rtc_read_time)(uint8_t reg, uint8_t *data) {
  uint8_t tmp_data;
  if (rtc_read(reg, &tmp_data))
    return 1;
  *data = ((tmp_data & 0xf0) >> 4) * 10 + (tmp_data & 0x0f);
  return 0;
}

int(rtc_write)(uint8_t reg, uint8_t data) {
  if (sys_outb(RTC_ADDR_REG, reg))
    return 1;
  if (sys_outb(RTC_DATA_REG, data))
    return 1;
  return 0;
}

int(rtc_enable_update_int)() {
  uint8_t reg_b = 0;
  if (rtc_read(RTC_REGISTER_B, &reg_b))
    return 1;
  reg_b |= RTC_REGISTER_B_UIE;
  // printf("reg_b = %x\n", reg_b);
  if (rtc_write(RTC_REGISTER_B, reg_b))
    return 1;
  // REGISTER_C must be read to clear these flags
  if (rtc_read(RTC_REGISTER_C, &reg_b))
    return 1;
  return 0;
}

void(rtc_ih)() {
  uint8_t reg_c = 0;
  if (rtc_read(RTC_REGISTER_C, &reg_c))
    return;
  if (reg_c & RTC_REGISTER_C_UF) {
    uint8_t attempts = 3;
    while (attempts) {
      uint8_t reg_a = 0;
      if (rtc_read(RTC_REGISTER_A, &reg_a))
        return;
      // loop while UIP is set to 1
      if (!(reg_a & RTC_REGISTER_A_UIP)) {
        rtc_read_time(RTC_HOURS, &rtc_time.hours);
        rtc_read_time(RTC_MINUTES, &rtc_time.minutes);
        rtc_read_time(RTC_SECONDS, &rtc_time.seconds);
        // printf("TIME = %d:%d:%d\n", rtc_time.hours, rtc_time.minutes, rtc_time.seconds);
      }
      attempts--;
      tickdelay(micros_to_ticks(244));
    }
  }
}
