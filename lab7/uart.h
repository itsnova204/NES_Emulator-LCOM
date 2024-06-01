#pragma once
#include <lcom/lcf.h>
#include "byteQueue.h"

#define BAUD_115200 1
#define BAUD_57600  2
#define BAUD_38400  3
#define BAUD_19200  6
#define BAUD_9600   12
#define BAUD_4800   24
#define BAUD_2400   48
#define BAUD_1200   96

#define NO_PARITY 0
#define ODD_PARITY BIT(3)
#define EVEN_PARITY BIT(3) | BIT(4)
#define C_PARITY_1 BIT(3) | BIT(5)
#define C_PARITY_2 BIT(3) | BIT(4) | BIT(5)


#define COM1_UART_ADDRESS 0x3F8
#define COM2_UART_ADDRESS 0x2F8

#define COM1_UART_IRQ 4
#define COM2_UART_IRQ 3

//register offsets
#define UART_RBR 0
#define UART_THR 0
#define UART_IER 1
#define UART_IIR 2
#define UART_FCR 2
#define UART_LCR 3
#define UART_MCR 4
#define UART_LSR 5
#define UART_MSR 6
#define UART_SR  7

#define DLL	    0x00  
#define DLM	    0x01  

//FIFO
#define FIFO_ENABLE         BIT(0)
#define FIFO_CLR_RCVR       BIT(1)
#define FIFO_CLR_SEND       BIT(2)
#define FIFO_ONE_BYTE       0
#define IIR_RECV_DATA_AVAIL BIT(2)
#define IIR_TRNSMT_EMPTY    BIT(1)
#define IIR_STATUS          BIT(1) | BIT(2)


//IER
#define ENABLE_RECEIVE_DATA_INTERRUPT BIT(0)
#define ENABLE_TRANSMIT_HOLDING_EMPTY_INTERRUPT BIT(1)
#define ENABLE_RECEIVER_LINE_STATUS_INTERRUPT BIT(2)
#define ENABLE_MODEM_STATUS_INTERRUPT BIT(3)

//LCR
#define WORD_LENGTH(n)  (n - 5)
#define STOP_BIT_1      0x00
#define STOP_BIT_2      BIT(2)
//parity on enum on top of file
#define LATCH_CONF      BIT(7)

//Line Status Register
#define DATA_READY BIT(0)
#define OVERRUN_ERROR BIT(1)
#define PARITY_ERROR BIT(2)
#define FRAMING_ERROR BIT(3)
#define BREAK_INTERRUPT BIT(4)
#define EMPTY_THR BIT(5)
#define EMPTY_DLL BIT(6)
#define FIFO_ERROR BIT(7)

int uart_init(uint8_t port, uint16_t rate, uint8_t word_length, uint8_t stop, uint8_t par);

int setBaud(uint8_t port, uint16_t rate);
void uart_ih(uint8_t port);

int uart_enable_fifo(uint8_t port);
int uart_set_IER(uint8_t port, uint8_t ctrl);

int uart_get_line_status(uint8_t port, uint8_t *status);

int uart_subscribe_int(uint8_t port, int *bit_no);

int uart_unsubscribe_int(uint8_t port);

int uart_enable_int(uint8_t port);
int uart_disable_int(uint8_t port);

int uart_exit(uint8_t port);

int uart_get_int_id(uint8_t port, uint8_t *id);
int uart_send_byte(uint8_t port, uint8_t byte);

bool uart_recv_front(uint8_t port, uint8_t *byte);

int uart_receive_byte(uint8_t port,uint8_t *byte);

void uart_push(uint8_t port, uint8_t byte);
