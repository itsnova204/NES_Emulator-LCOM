
#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "uart.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)() {
sp_init();

  uint8_t irq_set_sp;

  uint8_t byte;

  if (sp_subscribe_int(&irq_set_sp) != 0){
    printf("Error subscribing serial port\n");
    return 1;
  }
  sp_get_ctrl(&byte);
  printf("getting byte: %x\n",byte);
  byte |= BIT(0) | BIT(1); 
  printf("Setting byte: %x\n",byte);
  sys_outb(COM1_UART_BASE + LCR, byte);


  // SETUP e ciclo de interrupcoes do timer e do teclado

  printf("Starting loop\n");
  uint8_t times = 0;
  while( times < 10 ) {

      
      read_byte(&byte);
      printf("Received byte: %x\n",byte);
      usleep(10000);
      times++;
  }

  sp_unsubscribe_int();
  
  return 0;
}
