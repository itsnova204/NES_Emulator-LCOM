#include "keyboard.h"

int kbd_hook_id = 1;
static uint8_t scan_code;
static bool valid = false;

uint8_t(get_scan_code)() {
  return scan_code;
}

bool(is_valid)() {
  return valid;
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
  if (kbc_read_output(KBD_OUT_BUF, &scan_code, true)) {
    valid = false;
    return;
  }
  
  valid = true;
}

int(getScanCodeSize)(uint8_t scan_code) {
  if (scan_code == KBD_TWO_BYTE) return 2;
  return 1;
}

int (kbd_restore)() {
    uint8_t commandByte;

    if (kbc_write_command(KBD_IN_BUF, KBC_READ_CMD)) return 1;          
    if (kbc_read_output(KBD_OUT_BUF, &commandByte, true)) return 1;

    commandByte |= KBD_OUT_BUF_FULL;  

    if (kbc_write_command(KBD_IN_BUF, KBC_WRITE_CMD)) return 1;    
    if (kbc_write_command(KBC_WRITE_CMD, commandByte)) return 1;

    return 0;
}
