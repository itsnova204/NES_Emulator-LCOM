#include <lcom/lcf.h>
#define MOUSE_IRQ 12

static int hook_id = 12;

int (mouse_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL) return 1; 
  *bit_no = BIT(hook_id);       

  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (mouse_ih)() {
  kbc_read_output(true);
}

