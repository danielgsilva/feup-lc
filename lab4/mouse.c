#include "mouse.h"
#include <lcom/lcf.h>

static int mouse_hook_id;
uint8_t mouse_packet[3];
uint8_t mouse_packet_counter = 0;

int(mouse_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  mouse_hook_id = MOUSE_IRQ;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != 0) {
    printf("error: sys_irqsetpolicy()\n");
    return 1;
  }
  *bit_no = MOUSE_IRQ;
  return (mouse_hook_id == MOUSE_IRQ);
}

int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id) != 0) {
    printf("error: sys_irqrmpolicy()\n");
    return 1;
  }
  return 0;
}

void(mouse_ih)(void) {
  uint8_t mouse_packet_byte;
  if (kbc_read_out_buf(&mouse_packet_byte, 1) != 0) {
    printf("error: kbc_read_out_buf()\n");
    return;
  }
  // bit 3 of byte 1 of the packet must be 1
  if ((!mouse_packet_counter && (mouse_packet_byte & MOUSE_PACKET_FIRST_BYTE)) || mouse_packet_counter)
    mouse_packet[mouse_packet_counter++] = mouse_packet_byte;
}

void(mouse_parse_packet)(struct packet *pp) {
  for (int i = 0; i < mouse_packet_counter; i++) {
    pp->bytes[i] = mouse_packet[i];
  }

  pp->lb = (mouse_packet[0] & MOUSE_PACKET_LB);
  pp->rb = (mouse_packet[0] & MOUSE_PACKET_RB);
  pp->mb = (mouse_packet[0] & MOUSE_PACKET_MB);

  if (mouse_packet[0] & MOUSE_PACKET_MSB_X_DELTA)
    pp->delta_x = (mouse_packet[1] | 0xff00);
  else
    pp->delta_x = (mouse_packet[1] & 0x00ff);
  if (mouse_packet[0] & MOUSE_PACKET_MSB_Y_DELTA)
    pp->delta_y = (mouse_packet[2] | 0xff00);
  else
    pp->delta_y = (mouse_packet[2] & 0x00ff);

  pp->x_ov = (mouse_packet[0] & MOUSE_PACKET_X_OVFL);
  pp->y_ov = (mouse_packet[0] & MOUSE_PACKET_Y_OVFL);
}

int(mouse_data_reporting)(bool enable) {
  uint8_t attempts = 3, ack_byte;
  while (attempts) {
    if (kbc_write_command(KBC_IN_BUF_CMD, KBC_CMD_WRITE_BYTE_TO_MOUSE) != 0)
      return 1;

    if (enable) {
      if (kbc_write_command(KBC_IN_BUF_CMD_ARGS, MOUSE_ENABLE_DATA_REPORTING) != 0)
        return 1;
    }
    else if (kbc_write_command(KBC_IN_BUF_CMD_ARGS, MOUSE_DISABLE_DATA_REPORTING) != 0)
      return 1;
    if (kbc_read_out_buf(&ack_byte, 1) != 0)
      return 1;
    if (ack_byte == MOUSE_ACK)
      return 0;
    else if (ack_byte == MOUSE_NACK)
      attempts--;
    else if (ack_byte == MOUSE_ERROR) {
      printf("error: mouse controller sent ERROR\n()");
      return 1;
    }
  }
  printf("error: mouse_data_reporting() attempts exceeded\n");
  return 1;
}
