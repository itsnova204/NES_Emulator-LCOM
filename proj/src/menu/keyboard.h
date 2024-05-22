#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"
#include "KBC.h"

uint8_t(get_scan_code)();
bool(is_valid)();

int(kbd_subscribe_int)(uint8_t *bit_no);
int(kbd_unsubscribe_int)();
void(kbc_ih)();
int (kbd_restore)();

#endif
