
#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "uart.h"
#include "utils.h"

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



  uint8_t byte;



  sp_get_ctrl(&byte);
  printf("getting byte: %02x\n",byte);
  byte = BIT(0) | BIT(1) | BIT(7);//SELECT DL
  sys_outb(COM1_UART_BASE + LCR, byte);

  //The Divisor Latch (DL) register is a 16-bit register that contains the divisor for setting the bit-rate. I.e., in the PC, the bit-rate is obtained by dividing 115 200 by the contents of the DL register.

  sys_outb(COM1_UART_BASE + DLL, 0x01);//write dl
  sys_outb(COM1_UART_BASE + DLM, 0x00);

  byte = BIT(0) | BIT(1);
  sys_outb(COM1_UART_BASE + LCR, byte);

  //printf("Setting byte: %x\n",byte);
  sp_get_ctrl(&byte);
  printf("updated ctrl byte: %02x\n",byte);

  // SETUP e ciclo de interrupcoes do timer e do teclado
  byte = 0;
  printf("Starting loop\n");
  uint8_t times = 0;
  while( times < 10 ) {

      
      uint8_t status = read_byte(&byte);
      printf("Received byte: %02x stat%d\n",byte,status);
      usleep(10000);
      times++;
  }

  
  return 0;
}
