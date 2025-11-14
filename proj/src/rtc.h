#ifndef __RTC_H
#define __RTC_H

#include <lcom/lcf.h>
#include <stdint.h>

#define RTC_IRQ 8
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_SECONDS 0
#define RTC_MINUTES 2
#define RTC_HOURS 4
#define RTC_REGISTER_A 10
#define RTC_REGISTER_A_UIP BIT(7) // if set to 1, update in progress. Do not access time registers
#define RTC_REGISTER_B 11
#define RTC_REGISTER_B_UIE BIT(4)
#define RTC_REGISTER_C 12
#define RTC_REGISTER_C_UF BIT(4)

typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} RTC_Time;

int(rtc_subscribe_int)(uint8_t *bit_no);
int(rtc_unsubscribe_int)();

/**
 * @brief Read register reg of the RTC, 
 * writing the address of the register to the RTC_ADDR_REG 
 * and reading one byte from the RTC_DATA_REG.
*/
int(rtc_read)(uint8_t reg, uint8_t *data);

/**
 * @brief Lê o valor do registo reg (4, 2 ou 0) e converte de BCD para binário.
*/
int(rtc_read_time)(uint8_t reg, uint8_t *data);

/**
 * @brief Write on the register reg the RTC, 
 * writing the address of the register to the RTC_ADDR_REG 
 * and writing one byte to the RTC_DATA_REG.
*/
int(rtc_write)(uint8_t reg, uint8_t data);

/**
 * @brief Enable update interrupts (UI) using bit 4 (UIE) of REGISTER_B.
*/
int(rtc_enable_update_int)();

/**
 * @brief Lê a hora atual, lendo os registos das horas, minutos e segundos 
 * (registos 4, 2 e 0 respetivamente).
*/
void(rtc_ih)();

#endif
