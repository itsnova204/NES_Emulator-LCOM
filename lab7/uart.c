#include "uart.h"
#include "byteQueue.h"
#include "utils.h"

int uart_hook_id = 7;
int uart_irq = 0;
#define MAX_TRIES 5
#define DELAY 2000


static Queue *in_fifo_port1;
static Queue *out_fifo_port1;

static Queue *in_fifo_port2;
static Queue *out_fifo_port2;

int uart_init(uint8_t port, uint16_t rate, uint8_t word_length, uint8_t stop, uint8_t par){
    int base_addr;
    if(rate != 1 && rate != 2 && rate != 3 && rate != 6 && rate != 12 && rate != 24 && rate != 48 && rate != 96) return 1;

    if (port != 1 && port != 2) return 1;
    if (word_length < 5 || word_length > 8) return 1;
    if (stop != 1 && stop != 2) return 1;

    //checks ok -> init
    if(port == 1){
        base_addr = COM1_UART_ADDRESS;
        in_fifo_port1 = newQueue();
        out_fifo_port1 = newQueue();
    } 
    else if(port == 2){
        base_addr = COM2_UART_ADDRESS;
        in_fifo_port2 = newQueue();
        out_fifo_port2 = newQueue();
    } 
    else return 1;

    uint8_t ctrl_word = 0x00;
    util_sys_inb(base_addr + UART_LCR, &ctrl_word);
    ctrl_word |= WORD_LENGTH(word_length);
    ctrl_word |= par;
    if (stop == 2) ctrl_word |= STOP_BIT_2;
    if(sys_outb(COM1_UART_ADDRESS + UART_LCR, ctrl_word)) return 1;

    if(setBaud(port, rate)) return 1;
    


    //set DLAB back to 0
    if(sys_outb(base_addr + UART_LCR, ctrl_word & ~LATCH_CONF)) return 1;
    //ctrl word set

    uart_enable_fifo(port);
    return 0;
}

int setBaud(uint8_t port, uint16_t rate){
    int base_addr;
    if(rate != 1 && rate != 2 && rate != 3 && rate != 6 && rate != 12 && rate != 24 && rate != 48 && rate != 96) return 1;

    if (port != 1 && port != 2) return 1;

    //checks ok -> init
    if(port == 1){
        base_addr = COM1_UART_ADDRESS;
    } 
    else if(port == 2){
        base_addr = COM2_UART_ADDRESS;
    } 
    else return 1;

    //enable FIFO

    //create ctrl word
    uint8_t ctrl_word = 0x00;
    util_sys_inb(base_addr + UART_LCR, &ctrl_word);

    //set DLAB to 1 temporarily to set baud rate:
    if(sys_outb(base_addr + UART_LCR, ctrl_word | LATCH_CONF)) return 1;

    //set baud rate:
    uint8_t lsb, msb;
    if(util_get_LSB(rate, &lsb)) return 1;
    if(util_get_MSB(rate, &msb)) return 1;

    if(sys_outb(base_addr + UART_LSR, lsb)) return 1;
    if(sys_outb(base_addr + UART_MSR, msb)) return 1;

    //set DLAB back to 0
    if(sys_outb(base_addr + UART_LCR, ctrl_word & ~LATCH_CONF)) return 1;
    //ctrl word set

    return 0;
}

int uart_enable_fifo(uint8_t port){
    uint8_t fifo_conf = FIFO_ENABLE | FIFO_CLR_SEND | FIFO_CLR_RCVR | FIFO_ONE_BYTE;
    if (port == 1){
        return sys_outb(COM1_UART_ADDRESS + UART_FCR, fifo_conf);
    }else if (port == 2){
        return sys_outb(COM2_UART_ADDRESS + UART_FCR, fifo_conf);
    }
    return 1;
}

int uart_set_IER(uint8_t port, uint8_t ctrl){
    if      (port == 1)  return sys_outb(COM1_UART_ADDRESS + UART_IER, ctrl);
    else if (port == 2)  return sys_outb(COM2_UART_ADDRESS + UART_IER, ctrl);
    else return 1;
    return 0;
}

int uart_get_line_status(uint8_t port, uint8_t *status){
    int base_addr;
    if(port == 1)      base_addr = COM1_UART_ADDRESS;
    else if(port == 2) base_addr = COM2_UART_ADDRESS;
    else return 1;
    return util_sys_inb(base_addr + UART_LSR, status);
}

int uart_subscribe_int(uint8_t port, int *bit_no){
    *bit_no = BIT(uart_hook_id);  

    if (port == 1){
        return sys_irqsetpolicy(COM1_UART_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &uart_hook_id);
    }else if (port == 2){
        return sys_irqsetpolicy(COM2_UART_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &uart_hook_id);
    }
    return 1;
}

int uart_unsubscribe_int(uint8_t port){

    return sys_irqrmpolicy(&uart_hook_id);
}

void uart_ih(uint8_t port){
    if (port != 1 && port != 2) return;
    uint8_t err;
    uint8_t interrupt_id;
    uint8_t data;

    if (uart_get_int_id(port, &interrupt_id)) return;

    if (interrupt_id & IIR_RECV_DATA_AVAIL){
        while ((err = uart_receive_byte(port, &data)) != 1)
        {
            if(err == OK){

                if (port == 1){
                    push_byte(in_fifo_port1, data);
                }else if (port == 2){
                    push_byte(in_fifo_port2, data);
                }
            }
        }
    }

    if (interrupt_id & IIR_TRNSMT_EMPTY){
        if(port == 1){
            if (!isQueue_empty(out_fifo_port1)){
                uart_send_byte(port, queue_front(out_fifo_port1));
                queue_pop(out_fifo_port1);
            }
        }else if(port == 2){
            if (!isQueue_empty(out_fifo_port2)){
                uart_send_byte(port, queue_front(out_fifo_port2));
                queue_pop(out_fifo_port2);
            }
        }
    }
}

int uart_exit(uint8_t port){
    int base_addr;
    if(port == 1){
        base_addr = COM1_UART_ADDRESS;
        queue_delete(in_fifo_port1);
        queue_delete(out_fifo_port1);
    } 
    else if(port == 2){
        base_addr = COM2_UART_ADDRESS;
        queue_delete(in_fifo_port2);
        queue_delete(out_fifo_port2);
    }         
    else return 1;
    return sys_outb(base_addr + UART_FCR, 0); //disable FIFO
}

int uart_get_int_id(uint8_t port, uint8_t *id){
    int base_addr;
    if(port == 1) base_addr = COM1_UART_ADDRESS;
    else if(port == 2) base_addr = COM2_UART_ADDRESS;
    else return 1;
    return util_sys_inb(base_addr + UART_IIR, id);
}

int uart_send_byte(uint8_t port, uint8_t byte){
    uint8_t status; 
    uint8_t tries = 0;
    
    while (tries < MAX_TRIES){
        uart_get_line_status(port, &status);
        if ((status & EMPTY_THR)){//reg empty, ready to send
            return sys_outb(COM1_UART_ADDRESS + UART_THR, byte);
        }
        usleep(DELAY);
        tries++;
    }
    return 1;
}

void (queue_pop)(Queue* queue);
bool uart_recv_front(uint8_t port, uint8_t *byte){
    if(port == 1){
        if (isQueue_empty(in_fifo_port1)){
            return false;
        }
        *byte = queue_front(in_fifo_port1);
        queue_pop(in_fifo_port1);
    }else if(port == 2){
        if (isQueue_empty(in_fifo_port2)){
            return false;
        }
        *byte = queue_front(in_fifo_port2);
        queue_pop(in_fifo_port2);
    }
    return true;
}

int uart_receive_byte(uint8_t port,uint8_t *byte){
    uint8_t status, tries = 0;
    int err;
    
    while (tries < MAX_TRIES)
    {
        err = uart_get_line_status(port, &status);
        if (err) {
            tries++; micro_delay(DELAY); continue;
        }

        err = 1;
    
        if (status & DATA_READY)
        {
            err = util_sys_inb(COM1_UART_ADDRESS + UART_RBR, byte);
        }

        if (status & (OVERRUN_ERROR | PARITY_ERROR | FRAMING_ERROR))
        {
            return 2;
        }
        
        if(err == OK) return OK;
        tries++;
        usleep(DELAY);
    }

    return 1;
}

void uart_push(uint8_t port, uint8_t byte){
    if(port == 1){
        push_byte(out_fifo_port1, byte);
        
    }else if(port == 2){
        push_byte(out_fifo_port2, byte);
        
    }
}
