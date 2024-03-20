#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"
#include "util.h"
#include "keyboard.h"

struct kbc_status status_struct;

static int hook_id = 1;
static bool isInfiniteAttempts = false;
static uint8_t output = 0x00;

int (kbc_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL) return 1; 
  *bit_no = BIT(hook_id);       

  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

int kbc_setAttempts(int attempts){
  return 0;
}

void (kbc_ih)() {
  kbc_read_output(false);
}

uint8_t get_kbc_output(){
  return output;
}

void SetInfiniteAttempts(bool infinite){
  isInfiniteAttempts = infinite;
}

int kbc_read_output(bool lookingForMouse){
  int attempts = 3;
  
  while (attempts != 0 || isInfiniteAttempts){
    if (kbc_check_status() != 0){
      return -1;
    }
   
    if (status_struct.output_buffer_full && (status_struct.reading_mouse == lookingForMouse)){

      if(util_sys_inb(KBC_OUT_BUF, &output)) return -1;
       
      return 0;
    }

        tickdelay(micros_to_ticks(20000));
        attempts--;
  }

  printf("ERROR: KBC read out ran out of attempts\n");
  return -1;
}

int kbc_check_status(){
  uint8_t data = 0x00;

  if(util_sys_inb(KBC_STATUS_REG, &data) != 0){
    printf("ERROR: reading from keyboard status\n");
    return -1;
  } 

  if((data & KBC_AUX) != 0){   //check if it is outputing mouse data
    status_struct.reading_mouse = true;
  }else{
    status_struct.reading_mouse = false;
  }
  
  if ((data & KBC_IBF)){ //check if input buffer is full
    status_struct.input_buffer_full = true;
  }else{
    status_struct.input_buffer_full = false;
  }
  
  if((data & KBC_OUT_BUF_FULL) != 0){//check for output buffer full
    status_struct.output_buffer_full = true;

    if((data & KBC_ERR_PARITY) != 0){//check for parity error
      printf("ERROR: kbc output parity error\n");
      return PARITY_ERR;  
    } 
    if((data & KBC_ERR_TIMEOUT) != 0) {//check for timeout error
      printf("ERROR: kbc output timeout error\n");
      return TIMEOUT_ERR;  
    }   

  }else{
    status_struct.output_buffer_full = false;
  }

  return 0;
}

int kbc_write_command(uint8_t command, uint8_t port){

  while (true){
    if (kbc_check_status() != 0) return -1;
   
    if (status_struct.input_buffer_full == false){

      if(sys_outb(port, command)) return -1;
       
      return 0;
    }

    tickdelay(micros_to_ticks(20000));
  }
}

int kbc_restore(){

  if(kbc_write_command(KBC_READ_CMD, KBC_CMD_IN_REG) != 0) return 1;
  if(kbc_read_output(false) != 0) return 1; 

  if(kbc_write_command(KBC_SET_CMD, KBC_CMD_IN_REG) != 0) return 1;
  if(kbc_write_command(output | BIT(0), KBC_SEND_NEW_CMD) != 0) return 1;

  return 0;
}

