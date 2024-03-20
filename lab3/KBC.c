#include "KBC.h"

int(kbc_read_status)(uint8_t* status) {
  if (util_sys_inb(KBD_STAT_REG, status)) {
    return 1;
  }

  return 0;
}

int(kbc_read_output)(uint8_t port, uint8_t* output, bool max_attemps) {
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

