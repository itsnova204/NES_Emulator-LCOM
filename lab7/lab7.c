
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


int (proj_main_loop)()
{
    uint8_t ctrl;


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


   // init_uart_queues();
   // uart_init_fifo();

  //  uart_get_int_id(&ctrl);
   // int err = uart_set_interrupt_reg(ENABLE_LINE_STATUS | ENABLE_RECEIVE | ENABLE_TRANSMIT);


    uint8_t bit_no;

    if(sp_subscribe_int(&bit_no)) return 1;


    int uart_irq = BIT(bit_no);

    int ipc_status, r;
    message msg;


    uint8_t tries = 100;
    while (tries > 0)
    {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("Driver failed: %d\n", r);
            return r;
        }

        if (is_ipc_notify(ipc_status))
        {
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE:
                if (msg.m_notify.interrupts & uart_irq)
                {
                    uart_ih();
                }

                break;

            default:
                break;
            }
        }
        usleep(1000);
    }

    uart_set_interrupt_reg(0);
    err = sp_unsubscribe_int();
    if (err)
        return err;

}