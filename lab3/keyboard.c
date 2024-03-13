#include "keyboard.h"

int kbd_hook_id = 1;
uint8_t scan_code;

uint8_t(get_scan_code)() {
  return scan_code;
}

int(kbd_subscribe_int)(uint8_t *bit_no) {
  if( bit_no == NULL) return 1;   // validar o apontador

  *bit_no = BIT(kbd_hook_id);
  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0) return 1;

  return 0;
}

int(kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&kbd_hook_id) != 0) return 1;
  return 0;
}

void(kbd_int_handler)() {
  if (utils_sys_inb(KBD_STAT_REG, &scan_code) != 0) {
    printf("Keyboard ERROR: Couldn't read status\n");
    return;
  }

  if (scan_code & KBD_PARITY_ERR) {
    printf("Keyboard Parity error - invalid data\n");
    return;
  }

  if (scan_code & KBD_TIMEOUT_ERR) {
    printf("Keyboard Timeout error - invalid data\n");
    return;
  }

  if (scan_code & KBD_INHIBIT_FLAG) {
    printf("Keyboard Inhibit flag: 0 if keyboard is inhibited\n");
    return;
  }

  if (scan_code & KBD_OUT_BUF_FULL) {
    printf("Keyboard Output buffer full - data available for reading\n");
    return;
  }
}






