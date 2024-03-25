#include <lcom/lcf.h>

#include "mouse.h"
#include "i8042.h"
#include "keyboard.h"

#define MOUSE_IRQ 12

static int hook_id = 3;

uint8_t current_byte;
int byte_counter = 0;
uint8_t bytes[3];

struct packet mouse_packet; 

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

void mouse_reset_packet_counter(){
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
  return 0;
}

struct packet get_mouse_packet(){
  return mouse_packet;
}

void mouse_update_packet(){
  mouse_packet.y_ov = bytes[0] & MOUSE_Y_OVF;
  mouse_packet.x_ov = bytes[0] & MOUSE_X_OVF;

  mouse_packet.mb = bytes[0] & MOUSE_MIDLE;
  mouse_packet.rb = bytes[0] & MOUSE_RIGHT;
  mouse_packet.lb = bytes[0] & MOUSE_LEFT;

  if((bytes[0] & MOUSE_MSB_Y_DELTA) == 0){
    mouse_packet.delta_y = bytes[2];
  }else{
    mouse_packet.delta_y = bytes[2] - 256;
  }

  if((bytes[0] & MOUSE_MSB_X_DELTA) == 0){
    mouse_packet.delta_x = bytes[1];
  }else{
    mouse_packet.delta_x = bytes[1] - 256;
  }
}

int mouse_write_command(uint8_t command){

  uint8_t response = 0x00;
  uint8_t attempts = 3;

  while (attempts > 0){
    if(kbc_write_command(MOUSE_WRITE_CMD, KBC_CMD_IN_REG) != 0) return 1; //tell the kbc we are going to write a command to the mouse
    if (kbc_write_command(command, KBC_CMD_OUT_REG)) return 1; //comand sent!

    tickdelay(micros_to_ticks(20000));
    
    if (util_sys_inb(KBC_CMD_OUT_REG, &response)) return 1; //check response
    if (response == ACK) return 0;

    attempts--;
  }

  printf("ERROR: Mouse write command ran out of attempts\n");
  if (response == NACK) return 1;
  if (response == ERROR) return 1;
  return 1;
}
