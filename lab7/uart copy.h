#pragma once
//https://web.fe.up.pt/~pfs/aulas/lcom2013/at/8ser.pdf
#include <lcom/lcf.h>
#include "utils.h"

#define COM1_UART_BASE        0x3F8
#define COM2_UART_BASE        0x2F8

#define COM1_IRQ              4
#define COM2_IRQ              3

#define Rx_Buff_Reg           0x00
#define Tx_Holding_Reg        0x00
#define Interrupt_Enable_Reg  0x01
#define Interrupt_Id_Reg      0x02
#define FIFO_Ctrl_Reg         0x02
#define Line_Ctrl_Reg         0x03
#define Modem_Ctrl_Reg        0x04
#define Line_Status_Reg       0x05
#define Modem_Status_Reg      0x06
#define Scratchpad_Reg	      0x07 	

#define Divisor_Latch_LSB	    0x00  
#define Divisor_Latch_MSB	    0x01 

#define LSR_DATA_READY        BIT(0)
#define LSR_OVERRUN_ERROR     BIT(1)
#define LSR_PARITY_ERROR      BIT(2)
#define LSR_FRAMING_ERROR     BIT(3)
#define LSR_BREAK_INT         BIT(4)
#define LSR_THRE              BIT(5)
#define LSR_TEMT              BIT(6)
#define LSR_FIFO_ERROR        BIT(7)


int uart_subscribe_int(uint8_t port, uint8_t* bitno);
int uart_unsubscribe_int(uint8_t port);

int uart_get_status(uint8_t port ,uint8_t* status);
int uart_recive(uint8_t port, uint8_t* data);
int uart_send(uint8_t port, uint8_t data);


