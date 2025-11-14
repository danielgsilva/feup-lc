#include "action.h"

extern unsigned long timer0_counter;
extern uint8_t scancode;
extern uint8_t mouse_packet_counter;
extern struct packet pp;
extern MouseState mouse_state;
extern vbe_mode_info_t vmi_p;
extern RTC_Time rtc_time;

GameState game_state = NEW_GAME;
extern Move pawn_move;
extern bool dark_mode;

void(timer_action)() {
  timer_int_handler();
  if (timer0_counter % (sys_hz() / FRAME_RATE) == 0) {
    vg_double_buffering();
  }
}

void(kbd_action)() {
  kbd_ih();
  // printf("scancode: 0x%02x\n", scancode);
  mouse_state.mouse_mv = false;
  switch (scancode) {
    case ESC_BREAK_CODE:
      game_state = QUIT;
      break;
    case W_BREAK_CODE:
      if (game_state == CHOOSE_PAWN || game_state == CAPTURE)
        update_pawn_move(&pawn_move.xi, false);
      else if (game_state == MOVEMENT)
        update_pawn_move(&pawn_move.xf, false);
      break;
    case A_BREAK_CODE:
      if (game_state == CHOOSE_PAWN || game_state == CAPTURE)
        update_pawn_move(&pawn_move.yi, false);
      else if (game_state == MOVEMENT)
        update_pawn_move(&pawn_move.yf, false);
      break;
    case S_BREAK_CODE:
      if (game_state == CHOOSE_PAWN || game_state == CAPTURE)
        update_pawn_move(&pawn_move.xi, true);
      else if (game_state == MOVEMENT)
        update_pawn_move(&pawn_move.xf, true);
      break;
    case D_BREAK_CODE:
      if (game_state == CHOOSE_PAWN || game_state == CAPTURE)
        update_pawn_move(&pawn_move.yi, true);
      else if (game_state == MOVEMENT)
        update_pawn_move(&pawn_move.yf, true);
      break;
    case ENTER_BREAK_CODE:
      update_state_machine();
      break;
    default:
      break;
  }
  draw_frame();
}

void(mouse_action)() {
  mouse_ih();
  if (mouse_packet_counter == 3) {
    mouse_packet_counter = 0;
    mouse_parse_packet(&pp);
    // mouse_print_packet(&pp);

    int16_t x = mouse_state.x + pp.delta_x;
    int16_t y = mouse_state.y - pp.delta_y;
    if (x < 0 || y < 0 || x > vmi_p.XResolution || y > vmi_p.YResolution)
      return;
    mouse_state.mouse_mv = true;
    mouse_state.x = x;
    mouse_state.y = y;

    switch (game_state) {
      case NEW_GAME:
      case GAME_OVER:
        if (pp.lb)
          update_state_machine();
        break;
      case CHOOSE_PAWN:
      case CAPTURE:
        pawn_move.xi = find_board_cell(mouse_state.y);
        pawn_move.yi = find_board_cell(mouse_state.x);
        if (pp.lb)
          update_state_machine();
        break;
      case MOVEMENT:
        if (pp.lb) {
          pawn_move.xf = find_board_cell(mouse_state.y);
          pawn_move.yf = find_board_cell(mouse_state.x);
          update_state_machine();
        }
        break;
      default:
        break;
    }
    draw_frame();
  }
}

void(rtc_action)() {
  rtc_ih();
  if (rtc_time.hours < (DARK_MODE_HOUR - 12) || rtc_time.hours >= DARK_MODE_HOUR)
    dark_mode = true;
  else
    dark_mode = false;
}
