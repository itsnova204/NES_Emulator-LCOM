#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"
#include "util.h"

static int hook_id = 1;
static uint8_t scancode;

int counter = 0;

bool mouse_data_read = false;

uint8_t getKBCscancode(){
  return scancode;
}

int (kbc_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL) return 1; 
  *bit_no = BIT(hook_id);       

  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (kbc_ih)() {
  kbc_read_output(scancode, false);
}

int kbc_read_output(uint8_t output, bool reading_mouse){

  int attempts = 10;
  int status;

  while (attempts != 0){
      status = kbc_check_status();
      if (status < 0){
        printf("error reading from keyboard status\n");
        return 1;
      }
      
      if ((status & OUT_BUF_FULL) == 0)
      {
        continue;
      }
      

      switch (status){
      case OUT_BUF_FULL: //output buffer is ready to be read.
        if (mouse_data_read && reading_mouse){ //if data is for mouse and we are reading mouse data
          //todo
        }
          printf("reading kbc out buf\n");
        if(!mouse_data_read && !reading_mouse){ //if data is for keyboard and we are reading keyboard data
          if(util_sys_inb(KBC_OUT_BUF, &output) != 0){
            printf("error reading scancode\n");
            return 1;
          }
        }
        break;
      case PARITY_ERR:
        printf("KBC PARITY_ERR\n");
        return 1;
        break;
      case TIMEOUT_ERR:
        printf("KBC TIMEOUT_ERR\n");
        return 1;
        break;
      case IN_BUF_FULL:
        //todo
        break;
      case OUT_BUF_NOT_FULL:
        //todo
        break;
      
      default:
        break;
      }

      tickdelay(micros_to_ticks(20000)); //wait 20ms between atempts
      attempts--;
  }

  return 0;
}

int kbc_check_status(){
  uint8_t data = 0x00;

  if(util_sys_inb(KBC_OUT_BUF, &data) != 0) return -1;

  if((data & KBC_AUX) != 0){   //check if it is outputing mouse data
    mouse_data_read = true;
  }else{
    mouse_data_read = false;
  }
    
  if((data & KBC_ERR_PARITY) != 0)   return PARITY_ERR;  //check for parity error
  if((data & KBC_ERR_TIMEOUT) != 0)  return TIMEOUT_ERR;  //check for timeout error
  if((data & KBC_IBF) != 0)          return IN_BUF_FULL;  //check for input buffer full
  if((data & KBC_OUT_BUF_FULL) == 0) return OUT_BUF_NOT_FULL;  //check for output buffer full

  return OUT_BUF_FULL;
}
