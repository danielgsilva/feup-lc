#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include "i8042.h"
#include "kbd.h"

#include <stdbool.h>
#include <stdint.h>

extern uint8_t scancode;
extern uint32_t cnt; // Number of sys_inb() calls
extern unsigned long timer0_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no) != 0) {
    printf("error: kbd_subscribe_int()\n");
    return 1;
  }

  uint32_t irq_set = BIT(bit_no);

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
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            /* process it */
            kbc_ih();
            // printf("scancode: 0x%02x\n", scancode);
            if (scancode != TWO_BYTE_SCANCODE && !two_byte_scancode_flag) {
              bytes[0] = scancode;
              kbd_print_scancode(!(scancode & BREAK_CODE), 1, bytes);
            }
            else if (!two_byte_scancode_flag) {
              bytes[0] = scancode;
              two_byte_scancode_flag = 1;
            }
            else {
              bytes[1] = scancode;
              two_byte_scancode_flag = 0;
              kbd_print_scancode(!(scancode & BREAK_CODE), 2, bytes);
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

  return kbd_print_no_sysinb(cnt);
}

int(kbd_test_poll)() {
  uint8_t bytes[2];
  bool two_byte_scancode_flag = 0;
  while (scancode != ESC_BREAK_CODE) {
    // o BIT(5) (mouse data) deveria ser testado,
    // mas como serão utilizadas interrupções no projeto, não se alterou kbc_read_out_buf()
    if (!kbc_read_out_buf(&scancode)) {
      // printf("scancode: 0x%02x\n", scancode);
      if (scancode != TWO_BYTE_SCANCODE && !two_byte_scancode_flag) {
        bytes[0] = scancode;
        kbd_print_scancode(!(scancode & BREAK_CODE), 1, bytes);
      }
      else if (!two_byte_scancode_flag) {
        bytes[0] = scancode;
        two_byte_scancode_flag = 1;
      }
      else {
        bytes[1] = scancode;
        two_byte_scancode_flag = 0;
        kbd_print_scancode(!(scancode & BREAK_CODE), 2, bytes);
      }
    }
  }

  uint8_t cmd_byte;
  if (kbc_read_cmd_byte(&cmd_byte) != 0) {
    printf("error: kbc_read_cmd_byte()\n");
    return 1;
  }
  cmd_byte |= KBC_CMD_BYTE_INT;
  if (kbc_write_cmd_byte(cmd_byte) != 0) {
    printf("error: kbc_write_cmd_byte()\n");
    return 1;
  }

  return kbd_print_no_sysinb(cnt);
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t timer0_bit_no, kbd_bit_no;
  if (timer_subscribe_int(&timer0_bit_no) != 0) {
    printf("error: timer_subscribe_int()\n");
    return 1;
  }
  if (kbd_subscribe_int(&kbd_bit_no) != 0) {
    printf("error: kbd_subscribe_int()\n");
    return 1;
  }

  uint32_t timer0_int_bit = BIT(timer0_bit_no);
  uint32_t kbd_int_bit = BIT(kbd_bit_no);

  int ipc_status;
  message msg;
  uint8_t bytes[2], idle = 0;
  bool two_byte_scancode_flag = 0;

  while (scancode != ESC_BREAK_CODE && idle < n) { /* You may want to use a different condition */
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
              kbd_print_scancode(!(scancode & BREAK_CODE), 1, bytes);
            }
            else if (!two_byte_scancode_flag) {
              bytes[0] = scancode;
              two_byte_scancode_flag = 1;
            }
            else {
              bytes[1] = scancode;
              two_byte_scancode_flag = 0;
              kbd_print_scancode(!(scancode & BREAK_CODE), 2, bytes);
            }
            timer0_counter = 0;
            idle = 0;
          }
          if (msg.m_notify.interrupts & timer0_int_bit) { /* subscribed interrupt */
            /* process it */
            timer_int_handler();
            if ((timer0_counter % 60) == 0)
              idle++;
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

  return 0;
}
