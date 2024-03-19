#include "KBC.h"

int(kbc_read_status)(uint8_t* status) {
  if (util_sys_inb(KBD_STAT_REG, status)) {
    return 1;
  }
  return 0;
}

int(kbc_read_output)(uint8_t port, uint8_t* output) {
  uint8_t status;

  if (kbc_read_status(&status)) {
    printf("Keyboard ERROR: Couldn't read status\n");
    return 1;
  }

  if (status & KBD_OUT_BUF_FULL) {
    if (util_sys_inb(KBD_OUT_BUF, output)) {
      printf("Keyboard ERROR: Couldn't read output buffer\n");
      return 1;
    }
  }

  if (status & KBD_PARITY_ERR) {
    printf("Keyboard Parity error - invalid data\n");
    return 1;
  }

  if (status & KBD_TIMEOUT_ERR) {
    printf("Keyboard Timeout error - invalid data\n");
    return 1;
  }

  return 0;
}

int(kbc_write_command)(uint8_t port, uint8_t command) {
  uint8_t status;

  if (kbc_read_status(&status)) {
    printf("Keyboard ERROR: Couldn't read status\n");
    return 1;
  }

  if (status & KBD_IN_BUF_FULL) {
    printf("Keyboard ERROR: Input buffer is full\n");
    return 1;
  }

  if (sys_outb(port, command)) {
    printf("Keyboard ERROR: Couldn't write command\n");
    return 1;
  }

  return 0;     
}

