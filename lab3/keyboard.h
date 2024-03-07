#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "util.h"

uint8_t getKBCscancode();

int kbc_subscribe_int(uint8_t *bit_no);
int kbc_unsubscribe_int();
void (kbc_ih)();

int kbc_read_output(uint8_t output, bool reading_mouse);
int kbc_check_status();

#endif
