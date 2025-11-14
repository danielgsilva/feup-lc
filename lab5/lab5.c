// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "kbd.h"
#include "video.h"

extern uint8_t scancode;
extern unsigned long timer0_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (set_vbe_mode(mode))
    return 1;
  sleep(delay);
  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  if (vg_init(mode) == NULL)
    return 1;
  if (vg_draw_rectangle(x, y, width, height, color))
    return 1;

  uint8_t kbd_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no) != 0) {
    printf("error: kbd_subscribe_int()\n");
    return 1;
  }
  uint32_t kbd_int_bit = BIT(kbd_bit_no);
  int ipc_status;
  message msg;
  uint8_t bytes[2];
  bool two_byte_scancode_flag = 0;
  while (scancode != ESC_BREAK_CODE) { /* You may want to use a different condition */
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
            kbc_ih();
            // printf("scancode: 0x%02x\n", scancode);
            if (scancode != TWO_BYTE_SCANCODE && !two_byte_scancode_flag) {
              bytes[0] = scancode;
              // kbd_print_scancode(!(scancode & BREAK_CODE), 1, bytes);
            }
            else if (!two_byte_scancode_flag) {
              bytes[0] = scancode;
              two_byte_scancode_flag = 1;
            }
            else {
              bytes[1] = scancode;
              two_byte_scancode_flag = 0;
              // kbd_print_scancode(!(scancode & BREAK_CODE), 2, bytes);
            }
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
  if (kbd_unsubscribe_int() != OK) {
    printf("error: kbd_unsubscribe_int()\n");
    return 1;
  }

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (vg_init(mode) == NULL)
    return 1;
  if (vg_draw_pattern(mode, no_rectangles, first, step))
    return 1;

  uint8_t kbd_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no) != 0) {
    printf("error: kbd_subscribe_int()\n");
    return 1;
  }
  uint32_t kbd_int_bit = BIT(kbd_bit_no);
  int ipc_status;
  message msg;
  uint8_t bytes[2];
  bool two_byte_scancode_flag = 0;
  while (scancode != ESC_BREAK_CODE) { /* You may want to use a different condition */
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
            kbc_ih();
            // printf("scancode: 0x%02x\n", scancode);
            if (scancode != TWO_BYTE_SCANCODE && !two_byte_scancode_flag) {
              bytes[0] = scancode;
              // kbd_print_scancode(!(scancode & BREAK_CODE), 1, bytes);
            }
            else if (!two_byte_scancode_flag) {
              bytes[0] = scancode;
              two_byte_scancode_flag = 1;
            }
            else {
              bytes[1] = scancode;
              two_byte_scancode_flag = 0;
              // kbd_print_scancode(!(scancode & BREAK_CODE), 2, bytes);
            }
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
  if (kbd_unsubscribe_int() != OK) {
    printf("error: kbd_unsubscribe_int()\n");
    return 1;
  }

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (vg_init(INDEXED_COLOR_MODE) == NULL)
    return 1;

  if (vg_draw_pixmap(xpm, x, y))
    return 1;

  uint8_t kbd_bit_no;
  if (kbd_subscribe_int(&kbd_bit_no) != 0) {
    printf("error: kbd_subscribe_int()\n");
    return 1;
  }
  uint32_t kbd_int_bit = BIT(kbd_bit_no);
  int ipc_status;
  message msg;
  uint8_t bytes[2];
  bool two_byte_scancode_flag = 0;
  while (scancode != ESC_BREAK_CODE) { /* You may want to use a different condition */
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
            kbc_ih();
            // printf("scancode: 0x%02x\n", scancode);
            if (scancode != TWO_BYTE_SCANCODE && !two_byte_scancode_flag) {
              bytes[0] = scancode;
              // kbd_print_scancode(!(scancode & BREAK_CODE), 1, bytes);
            }
            else if (!two_byte_scancode_flag) {
              bytes[0] = scancode;
              two_byte_scancode_flag = 1;
            }
            else {
              bytes[1] = scancode;
              two_byte_scancode_flag = 0;
              // kbd_print_scancode(!(scancode & BREAK_CODE), 2, bytes);
            }
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
  if (kbd_unsubscribe_int() != OK) {
    printf("error: kbd_unsubscribe_int()\n");
    return 1;
  }

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  if ((xi != xf && yi != yf) || xi > xf || yi > yf)
    return 1;
  if (vg_init(INDEXED_COLOR_MODE) == NULL)
    return 1;

  uint8_t kbd_bit_no, timer0_bit_no;
  if (timer_subscribe_int(&timer0_bit_no) != 0) {
    printf("error: timer_subscribe_int()\n");
    return 1;
  }
  if (kbd_subscribe_int(&kbd_bit_no) != 0) {
    printf("error: kbd_subscribe_int()\n");
    return 1;
  }

  if (vg_draw_pixmap(xpm, xi, yi))
    return 1;
  vg_double_buffering();

  uint32_t kbd_int_bit = BIT(kbd_bit_no);
  uint32_t timer0_int_bit = BIT(timer0_bit_no);
  int ipc_status;
  message msg;
  uint8_t bytes[2];
  bool two_byte_scancode_flag = 0;
  while (scancode != ESC_BREAK_CODE) { /* You may want to use a different condition */
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
            kbc_ih();
            // printf("scancode: 0x%02x\n", scancode);
            if (scancode != TWO_BYTE_SCANCODE && !two_byte_scancode_flag) {
              bytes[0] = scancode;
              // kbd_print_scancode(!(scancode & BREAK_CODE), 1, bytes);
            }
            else if (!two_byte_scancode_flag) {
              bytes[0] = scancode;
              two_byte_scancode_flag = 1;
            }
            else {
              bytes[1] = scancode;
              two_byte_scancode_flag = 0;
              // kbd_print_scancode(!(scancode & BREAK_CODE), 2, bytes);
            }
          }
          if (msg.m_notify.interrupts & timer0_int_bit) { /* subscribed interrupt */
            /* process it */
            timer_int_handler();
            if (timer0_counter % (sys_hz() / fr_rate) == 0) {
              if (yi != yf) { // vertical movement
                if (speed > 0)
                  yi += speed;
                else if ((timer0_counter % abs(speed)) == 0)
                  yi++;
                if (yi > yf)
                  yi = yf;

                vg_fill(0);
                if (vg_draw_pixmap(xpm, xi, yi))
                  return 1;
                vg_double_buffering();
              }
              else if (xi != xf) { // horizontal movement
                if (speed > 0)
                  xi += speed;
                else if ((timer0_counter % abs(speed)) == 0)
                  xi++;
                if (xi > xf)
                  xi = xf;

                vg_fill(0);
                if (vg_draw_pixmap(xpm, xi, yi))
                  return 1;
                vg_double_buffering();
              }
            }
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
  if (timer_unsubscribe_int() != OK) {
    printf("error: timer_unsubscribe_int()\n");
    return 1;
  }
  if (kbd_unsubscribe_int() != OK) {
    printf("error: kbd_unsubscribe_int()\n");
    return 1;
  }

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
