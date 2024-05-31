/**
 * @file mouse.h
 * @brief This file contains the implementation of the mouse functions
 * 
*/

#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "i8042.h"
#include "kbc.h"

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

void (mouse_int_handler)();

bool (mouse_sync)();

void (mouse_parse_packet)();

int (mouse_bytes_to_packet)();

int (mouse_write)(uint8_t cmd);

struct packet (get_mouse_packet)();

# endif
