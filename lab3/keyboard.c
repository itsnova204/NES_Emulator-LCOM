#include <lcom/lcf.h>
//#include <lcom/keyboard.h>
#include <stdint.h>
#include "i8042.h"

int kbc_hook_id = 1;

int(kbc_subscribe_int)(uint8_t *bit_no) {
  if( bit_no == NULL) return 1;   // validar o apontador

  *bit_no = BIT(kbc_hook_id);
  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE, &kbc_hook_id) != 0) return 1;

  return 0;
}

int(kbc_unsubscribe_int)() {
  return 0;
}



