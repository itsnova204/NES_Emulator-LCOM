#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>


#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "uart.h"


extern int timeCounter;


// DEFINE FPS
#define FPS 60
#define FRAME_INTERVAL (60 / FPS)

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab7/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab7/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

#define MAKE_UP   0x4B
#define BREAK_UP  0xCB
#define MAKE_LEFT 0x4D
#define BREAK_LEFT 0xCD
#define MAKE_RIGHT 0x4D
#define BREAK_RIGHT 0xCD
#define MAKE_DOWN 0x50
#define BREAK_DOWN 0xD0

#define MAKE_A 0x1E
#define BREAK_A 0x9E
#define MAKE_S 0x1F
#define BREAK_S 0x9F

#define rate BAUD_115200

int (proj_main_loop)(){
    uint8_t ctrl;
    uint8_t port = 1;

    uart_init(port, rate, 8, 1, ODD_PARITY);
    uart_get_int_id(port,&ctrl);
    if ((ctrl & IIR_RECV_DATA_AVAIL))
    {
        uart_ih(port);
    }

    if(uart_set_IER(port, ENABLE_RECEIVER_LINE_STATUS_INTERRUPT | ENABLE_RECEIVE_DATA_INTERRUPT)) return 1;

    int bit_no;
    uint8_t bit;

    if(timer_subscribe_int(&bit)) return 1;
    int timer_irq = BIT(bit);


    if(uart_subscribe_int(port,&bit_no)) return 1;
    int uart_irq = BIT(bit_no);

    int ipc_status, r;
    message msg;
    int timeCounter = 0;
    printf("Start game!\n");
    while (timeCounter < 10 * 60)
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
                if (msg.m_notify.interrupts & timer_irq)
                {
                    timeCounter++;
                    uint8_t byte;

                    while (uart_recv_front(port,&byte))
                    {
                        printf("byte yes: %d\n", byte);
                    }
                }

                if (msg.m_notify.interrupts & uart_irq)
                {
                    uart_ih(port);
                }

                break;

            default:
                break;
            }
        }
    }

    uart_set_IER(port,0);

    if(uart_unsubscribe_int(port)) return 1;
    return timer_unsubscribe_int();
}
