#include "uart.h"

int sp_hook_id = 7;


int sp_subscribe_int(uint8_t* bitno){
    *bitno = BIT(sp_hook_id);
    if(sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &sp_hook_id)) return 1;
    return 0;
}

int sp_unsubscribe_int(){
    if(sys_irqrmpolicy(&sp_hook_id)) return 1;
    return 0;
}

int sp_get_status(uint8_t* status){
    return util_sys_inb(COM1_UART_BASE + LSR, status);
}

int sp_get_ctrl(uint8_t* status){
    return util_sys_inb(COM1_UART_BASE + LCR, status);
}


void sp_init(){
    uint8_t ier;
    if(util_sys_inb(COM1_UART_BASE+ IER, &ier)) return;
    ier &= 0xF0;
    if(sys_outb(COM1_UART_BASE + IER,ier | IER_ERBFI)) return;
}

int send_byte(uint8_t byte){
    uint8_t status, attempts = 10;
    while(attempts--){
      if(sp_get_status(&status)) return 1;
      if(status & LSR_THRE)
        return sys_outb(COM1_UART_BASE + THR,byte);
    }
    return 1;
}

int read_byte(uint8_t* byte){
    uint8_t status;
    if(sp_get_status(&status)) return 1;
    if(status & LSR_DATA_READY){
      if(util_sys_inb(COM1_UART_BASE + RBR, byte)) return 1;
      if(!(status & (LSR_OE| LSR_PE| LSR_FE))){
          return 0;
      }
    }
    return 1;
}

int SP_clearInterrupts(){
    if(sys_outb(COM1_UART_BASE + FCR,FCR_CLEAR)) return 1;

    return 0;
}

void sp_ih(uint8_t* byte){
    uint8_t iir;
    if(util_sys_inb(COM1_UART_BASE+ IIR, &iir)) return;
    if(!(iir & IIR_NO_INT_PENDING))
        if(iir & IIR_ID == IIR_FIFO_CT)
            while(read_byte(byte)){
                printf("Received byte: %x\n",*byte);
            };
}

