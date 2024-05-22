#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "i8042.h"

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

void (mouse_ih)();

void mouse_reset_packet_counter();

int mouse_store_bytes();

struct packet get_mouse_packet();

void mouse_update_packet();

int mouse_write_command(uint8_t command);

#endif 
