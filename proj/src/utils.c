#include <lcom/lcf.h>
#include <stdint.h>

int(util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL)
    return 1;
  uint32_t tmp_value;
  if (sys_inb(port, &tmp_value) != 0) {
    printf("error: sys_inb()\n");
    return 1;
  }
  *value = (uint8_t) tmp_value;

  return 0;
}
