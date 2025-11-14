// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "action.h"
#include "claustro.h"
#include "kbd.h"
#include "mouse.h"
#include "rtc.h"
#include "video.h"

extern GameState game_state;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(proj_main_loop)(int argc, char **argv) {
  /* your main loop code here */
  if (vg_init(DIRECT_COLOR_MODE_8_8_8) == NULL)
    return 1;
  create_sprites();

  setup_board();
  draw_frame();

  uint8_t kbd_bit_no, timer0_bit_no, mouse_bit_no, rtc_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no) != 0) {
    printf("error: kbd_subscribe_int()\n");
    return 1;
  }
  if (timer_subscribe_int(&timer0_bit_no) != 0) {
    printf("error: timer_subscribe_int()\n");
    return 1;
  }
  if (mouse_subscribe_int(&mouse_bit_no) != 0) {
    printf("error: mouse_subscribe_int()\n");
    return 1;
  }
  if (rtc_subscribe_int(&rtc_bit_no) != 0) {
    printf("error: rtc_subscribe_int()\n");
    return 1;
  }
  uint32_t kbd_int_bit = BIT(kbd_bit_no);
  uint32_t timer0_int_bit = BIT(timer0_bit_no);
  uint32_t mouse_int_bit = BIT(mouse_bit_no);
  uint32_t rtc_int_bit = BIT(rtc_bit_no);

  if (mouse_data_reporting(1) != 0) {
    printf("error: mouse_data_reporting()\n");
    return 1;
  }
  if (rtc_enable_update_int()) {
    printf("error: rtc_enable_update_int()\n");
    return 1;
  }

  int ipc_status;
  message msg;
  while (game_state != QUIT) {
    /* Get a request message. */
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & kbd_int_bit) { /* subscribed interrupt */
            /* process it */
            kbd_action();
          }
          if (msg.m_notify.interrupts & timer0_int_bit) { /* subscribed interrupt */
            /* process it */
            timer_action();
          }
          if (msg.m_notify.interrupts & mouse_int_bit) { /* subscribed interrupt */
            /* process it */
            mouse_action();
          }
          if (msg.m_notify.interrupts & rtc_int_bit) { /* subscribed interrupt */
            /* process it */
            rtc_action();
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  if (vg_exit())
    return 1;
  destroy_sprites();

  if (kbd_unsubscribe_int() != 0) {
    printf("error: kbd_unsubscribe_int()\n");
    return 1;
  }
  if (timer_unsubscribe_int() != 0) {
    printf("error: timer_unsubscribe_int()\n");
    return 1;
  }
  if (mouse_unsubscribe_int() != 0) {
    printf("error: mouse_unsubscribe_int()\n");
    return 1;
  }
  if (rtc_unsubscribe_int() != 0) {
    printf("error: rtc_unsubscribe_int()\n");
    return 1;
  }

  if (mouse_data_reporting(0) != 0) {
    printf("error: mouse_data_reporting()\n");
    return 1;
  }

  return 0;
}
