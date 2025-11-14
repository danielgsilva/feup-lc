#include <lcom/lcf.h>
#include <stdint.h>

uint32_t cnt = 0;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL)
    return 1;
  *lsb = (uint8_t) val;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL)
    return 1;
  val >>= 8;
  *msb = (uint8_t) val;

  return 0;
}

int(util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL)
    return 1;
  uint32_t tmp_value;
  if (sys_inb(port, &tmp_value) != 0) {
    printf("error: sys_inb()\n");
    return 1;
  }
  *value = (uint8_t) tmp_value;

#ifdef LAB3
  cnt++;
#endif

  return 0;
}
