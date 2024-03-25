#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "util.h"

struct kbc_status_struct{
  bool isOutputMouse;
  bool isInputBufferFull;
  bool isOutputBufferFull;
};

int (kbc_subscribe_int)(uint8_t *bit_no);

int (kbc_unsubscribe_int)();

void (kbc_ih)();

uint8_t get_kbc_output();

void set_isInfiniteAttempts(bool infinite);

int kbc_read_output(bool lookingForMouse);

int kbc_check_status();

int kbc_restore();

int kbc_write_command(uint8_t command, uint8_t port);

#endif //_KEYBOARD_H_

