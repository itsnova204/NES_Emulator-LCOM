#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"

int(getScanCodeSize)(uint8_t scan_code);
bool(is_two_byte_scan_code)(uint8_t scan_code);
int print_scancode(uint8_t scan_code);

int(kbc_read_output)(uint8_t port, uint8_t *output, uint8_t mouse);
int(kbc_write_command)(uint8_t port, uint8_t command);

#endif
