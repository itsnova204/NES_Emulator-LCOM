/**
 * @file KBC.h
 * @brief This file contains the implementation of the KBC functions allowing handling of the keyboard and mouse interrupts
 * 
*/

#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"

int(getScanCodeSize)(uint8_t scancode);
bool(is_two_byte_scancode)(uint8_t scancode);
int print_scancode(uint8_t scancode);

int(kbc_read_output)(uint8_t port, uint8_t *output, uint8_t mouse);
int(kbc_write_command)(uint8_t port, uint8_t command);

#endif
