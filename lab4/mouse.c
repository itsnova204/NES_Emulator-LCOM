#include <lcom/lcf.h>

#include "mouse.h"
#include "i8042.h"
#include "keyboard.h"

#define MOUSE_IRQ 12

static int hook_id = 12;

uint8_t current_byte;
int byte_counter = 0;
uint8_t bytes[3];

struct packet packet; 

int (mouse_subscribe_int)(uint8_t *bit_no){
  if(bit_no == NULL) return 1; 
  *bit_no = BIT(hook_id);       

  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (mouse_unsubscribe_int)(){
  return sys_irqrmpolicy(&hook_id);
}

void (mouse_ih)(){
  kbc_read_output(true);
  current_byte = get_kbc_output();
}

void reset_mouse_packet_counter(){
  byte_counter = 0;
}

int mouse_store_bytes(){
  if(byte_counter == 0 && ((current_byte & BIT(3)) != 0)){
    bytes[0] = current_byte;
    byte_counter++;
    return 1;
  }

  if (byte_counter == 1){
    bytes[1] = current_byte;
    byte_counter++;
    return 2;
  }

  if (byte_counter == 2){
    bytes[2] = current_byte;
    byte_counter++;
    mouse_update_packet();
    return 3;
  }
}

struct packet get_mouse_packet(){
  return packet;
}

void mouse_update_packet(){
  packet.y_ov = bytes[0] & MOUSE_Y_OVF;
  packet.x_ov = bytes[0] & MOUSE_X_OVF;

  packet.mb = bytes[0] & MOUSE_MIDLE;
  packet.rb = bytes[0] & MOUSE_RIGHT;
  packet.lb = bytes[0] & MOUSE_LEFT;

  if((bytes[0] & MOUSE_MSB_Y_DELTA) == 0){
    packet.delta_y = bytes[2];
  }else{
    packet.delta_y = bytes[2] - 256;
  }

  if((bytes[0] & MOUSE_MSB_X_DELTA) == 0){
    packet.delta_x = bytes[1];
  }else{
    packet.delta_x = bytes[1] - 256;
  }
}
