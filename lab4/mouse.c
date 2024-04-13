#include "mouse.h"

int mouse_hook_id = 2;
uint8_t byte_index = 0;
uint8_t mouse_bytes[3];
uint8_t current_byte;

struct packet mouse_packet;

int (mouse_subscribe_int)(uint8_t *bit_no){
  if (bit_no == NULL) return 1;

  *bit_no = BIT(mouse_hook_id);
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id)) return 1;

  return 0;
}

int (mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id)) return 1;
  return 0;
}


