#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "i8042.h"
#include "i8254.h"
#include "KBC.c"
#include "mouse.h"

struct packet mouse_packet;
uint8_t byte_index;

typedef enum {
  START,
  UP,
  VERTEX,
  DOWN,
  END
} SystemState;

SystemState state = START;
uint16_t x_len_total = 0;


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status;
  message msg;
  uint8_t mouse_mask;

  if (mouse_subscribe_int(&mouse_mask)) return 1;

  if (mouse_enable_data_reporting()) return 1;

  while (cnt) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0){
      printf("Mouse driver_receive failed");
      continue;
    }

    if (is_ipc_notify(ipc_status)){
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE: 
          if (msg.m_notify.interrupts & mouse_mask){
            byte_index = getByteIndex();
            mouse_ih();        
            if (!is_valid()) continue;

            mouse_sync_bytes();
            
            if (byte_index == 3) {               
              mouse_bytes_to_packet();               
              mouse_print_packet(&mouse_packet);
              resetByteIndex();
              cnt--;
            }
          }
          break;
      }
    }
  }
  
  if (mouse_write(MOUSE_DISABLE_DATA_REPORT)) return 1;

  if (mouse_unsubscribe_int()) return 1;
 
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
