#include "keyboard.h"

int kbd_hook_id = 1;
static uint8_t scan_code;
static bool valid = true;

bool isScanCodeValid() {
  return valid;
}

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

void(kbc_ih)() {
  uint8_t status;

  if (util_sys_inb(KBD_STAT_REG, &status) != 0) {
    printf("Keyboard ERROR: Couldn't read status\n");
    valid = false;
    return;
  }

  if (status & KBD_OUT_BUF_FULL) {
    if (util_sys_inb(KBD_OUT_BUF, &scan_code) != 0) {
      printf("Keyboard ERROR: Couldn't read output buffer\n");
      return;
    }
    valid = true;
  }

  if (status & KBD_PARITY_ERR) {
    printf("Keyboard Parity error - invalid data\n");
    valid = false;
  }

  if (status & KBD_TIMEOUT_ERR) {
    printf("Keyboard Timeout error - invalid data\n");
    valid = false;
  }
}

