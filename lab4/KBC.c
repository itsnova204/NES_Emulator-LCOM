#include "KBC.h"

static uint8_t scan_code_array[2];
static bool flag_two_byte = false;
static int index_ = 0;

int(kbc_read_status)(uint8_t* status) {
  if (util_sys_inb(KBD_STAT_REG, status)) {
    return 1;
  }

  return 0;
}

int(kbc_read_output)(uint8_t port, uint8_t* output, bool max_attemps, uint8_t mouse) {
  uint8_t status;
  int attemps = MAX_ATTEMPS;

  while (!max_attemps || attemps > 0) {
    if (kbc_read_status(&status)) {
      printf("Keyboard ERROR: Couldn't read status\n");
      return 1;
    }

    if (status & KBD_OUT_BUF_FULL) {
      if (util_sys_inb(KBD_OUT_BUF, output)) {
        printf("Keyboard ERROR: Couldn't read output buffer\n");
        return 1;
      }

      if (status & KBD_PARITY_ERR) {
        printf("Keyboard Parity error - invalid data\n");
        return 1;
      }

      if (status & KBD_TIMEOUT_ERR) {
        printf("Keyboard Timeout error - invalid data\n");
        return 1;
      }

      if (mouse && !(status & BIT(5))) {
        printf("Mouse data not valid\n");
        return 1;
      }

      if (!mouse && (status & BIT(5))) {
        printf("Keyboard data not valid\n");
        return 1;
      }

      return 0;
    }

    tickdelay(micros_to_ticks(DELAY));
    attemps--;
  }

  return 1;
}

int(kbc_write_command)(uint8_t port, uint8_t command) {
  uint8_t status;
  int attemps = MAX_ATTEMPS;

  while (attemps > 0) {
    if (kbc_read_status(&status)) {
      printf("Keyboard ERROR: Couldn't read status\n");
      return 1;
    }

    if (!(status & KBD_IN_BUF_FULL)) {
      if (sys_outb(port, command)) {
        printf("Keyboard ERROR: Couldn't write command\n");
        return 1;
      }

      return 0;
    }
    tickdelay(micros_to_ticks(DELAY));
    attemps--;
  }

  return 1;
}

int(getScanCodeSize)(uint8_t scan_code) {
  if (scan_code == KBD_TWO_BYTE) return 2;
  return 1;
}

bool is_two_byte_scan_code(uint8_t scan_code) {
    return (getScanCodeSize(scan_code) == 2);
}

int print_scancode(uint8_t scan_code) {
  if (is_two_byte_scan_code(scan_code) && !flag_two_byte) {
    scan_code_array[index_++] = scan_code;
    flag_two_byte = true;
    return 0;
  } 
    
  if (flag_two_byte) {
    flag_two_byte = false;
    scan_code_array[index_] = scan_code;
    index_ = 0;
    if (kbd_print_scancode(!(KBD_MAKE_CODE & scan_code_array[1]), 2, scan_code_array)) return 1;
    memset(scan_code_array, 0, 2);
    return 0;
  }
    
  if (kbd_print_scancode(!(KBD_MAKE_CODE & scan_code), 1, &scan_code)) return 1;

  return 0;
}

