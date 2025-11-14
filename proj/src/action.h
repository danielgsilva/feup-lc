#ifndef __ACTION_H
#define __ACTION_H

#include "claustro.h"
#include "kbd.h"
#include "mouse.h"
#include "rtc.h"
#include "video.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  NEW_GAME,
  CHOOSE_PAWN,
  MOVEMENT,
  CAPTURE,
  GAME_OVER,
  QUIT
} GameState;

/**
 * @brief Processa uma interrupção do timer chamando o seu interrupt handler
 * e executa double-buffering à taxa definida.
*/
void(timer_action)();

/**
 * @brief Processa uma interrupção do teclado chamando o seu interrupt handler
 * e tratando a informação de acordo com estado do jogo.
*/
void(kbd_action)();

/**
 * @brief Processa uma interrupção do rato chamando o seu interrupt handler
 * e tratando a informação de acordo com estado do jogo.
*/
void(mouse_action)();

/**
 * @brief Processa uma interrupção do RTC, chamando o seu interrupt handler
 * e ativa/desativa o dark mode de acordo com a hora.
*/
void(rtc_action)();

#endif
