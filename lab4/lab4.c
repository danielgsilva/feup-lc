// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "mouse.h"

extern uint8_t mouse_packet[3];
extern uint8_t mouse_packet_counter;
extern unsigned long timer0_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  // if (mouse_enable_data_reporting())
  //   return 1;
  uint8_t mouse_bit_no;
  if (mouse_subscribe_int(&mouse_bit_no) != 0) {
    printf("error: mouse_subscribe_int()\n");
    return 1;
  }
  if (mouse_data_reporting(1) != 0) {
    printf("error: mouse_data_reporting()\n");
    return 1;
  }


  uint32_t mouse_int_bit = BIT(mouse_bit_no);

  int ipc_status;
  message msg;
  struct packet pp;
  while (cnt) { /* You may want to use a different condition */
    /* Get a request message. */
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                   /* hardware interrupt notification */
          if (msg.m_notify.interrupts & mouse_int_bit) { /* subscribed interrupt */
            /* process it */
            mouse_ih();
            if (mouse_packet_counter == 3) {
              mouse_parse_packet(&pp);
              mouse_print_packet(&pp);
              mouse_packet_counter = 0;
              cnt--;
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

  if (mouse_data_reporting(0) != 0) {
    printf("error: mouse_data_reporting()\n");
    return 1;
  }
  if (mouse_unsubscribe_int() != 0) {
    printf("error: mouse_unsubscribe_int()\n");
    return 1;
  }


  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  // if (mouse_enable_data_reporting())
  //   return 1;
  if (mouse_data_reporting(1) != 0) {
    printf("error: mouse_data_reporting()\n");
    return 1;
  }

  uint8_t mouse_bit_no, timer0_bit_no;
  if (mouse_subscribe_int(&mouse_bit_no) != 0) {
    printf("error: mouse_subscribe_int()\n");
    return 1;
  }
  if (timer_subscribe_int(&timer0_bit_no) != 0) {
    printf("error: timer_subscribe_int()\n");
    return 1;
  }

  uint32_t mouse_int_bit = BIT(mouse_bit_no);
  uint32_t timer0_int_bit = BIT(timer0_bit_no);

  int ipc_status;
  message msg;
  struct packet pp;
  uint8_t idle_time_tmp = 0;
  while (idle_time_tmp < idle_time) { /* You may want to use a different condition */
    /* Get a request message. */
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                   /* hardware interrupt notification */
          if (msg.m_notify.interrupts & mouse_int_bit) { /* subscribed interrupt */
            /* process it */
            mouse_ih();
            if (mouse_packet_counter == 3) {
              mouse_parse_packet(&pp);
              mouse_print_packet(&pp);
              mouse_packet_counter = 0;
            }
            timer0_counter = 0;
            idle_time_tmp = 0;
          }
          if (msg.m_notify.interrupts & timer0_int_bit) { /* subscribed interrupt */
            /* process it */
            timer_int_handler();
            if ((timer0_counter % sys_hz()) == 0){
              idle_time_tmp++;
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

  if (mouse_unsubscribe_int() != 0) {
    printf("error: mouse_unsubscribe_int()\n");
    return 1;
  }
  if (timer_unsubscribe_int() != 0) {
    printf("error: timer_unsubscribe_int()\n");
    return 1;
  }

  if (mouse_data_reporting(0) != 0) {
    printf("error: mouse_data_reporting()\n");
    return 1;
  }

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
