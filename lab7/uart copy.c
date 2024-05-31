#include "uart.h"
int com1_hook_id = 4;
int com2_hook_id = 3;

int uart_subscribe_int(uint8_t port, uint8_t* bitno){
    if(port == 1)
        *bitno = BIT(com1_hook_id);
    else if(port == 2)
        *bitno = BIT(com2_hook_id);
    else{
        printf("[UART] Invalid port\n");
        return 1;
    };

    return sys_irqsetpolicy((port == 1 ? COM1_IRQ : COM2_IRQ), IRQ_REENABLE | IRQ_EXCLUSIVE, (port == 1 ? &com1_hook_id : &com2_hook_id));
}

int uart_unsubscribe_int(uint8_t port){
    int* temp_uart = (port == 1 ? &com1_hook_id : &com2_hook_id);
    return sys_irqrmpolicy(temp_uart);
}

int uart_get_status(uint8_t port ,uint8_t* status){
  switch (port){
  case 1:
    if(util_sys_inb(COM1_UART_BASE + Line_Status_Reg, status) != 0){
      printf("[UART] Error reading Line Status Register\n");
      return 1;
    }
    break;
  case 2:
    if(util_sys_inb(COM2_UART_BASE + Line_Status_Reg, status) != 0){
      printf("[UART] Error reading Line Status Register\n");
      return 1;
    }
    break;

  default:
    printf("[UART] Invalid port\n");
    return 1;
    break;
  }
  return 0;
}

int uart_recive(uint8_t port, uint8_t* data){
  uint8_t status;
  if(uart_get_status(port, &status) != 0){
    printf("[UART] Error getting status\n");
    return 1;
  }
  if((status & LSR_DATA_READY) == 0){
    printf("[UART] Data not ready\n");
    return 1;
  }
  switch (port){
  case 1:
    if(util_sys_inb(COM1_UART_BASE + Rx_Buff_Reg, data) != 0){
      printf("[UART] Error reading Rx Buffer Register\n");
      return 1;
    }
    break;
  case 2:
    if(util_sys_inb(COM2_UART_BASE + Rx_Buff_Reg, data) != 0){
      printf("[UART] Error reading Rx Buffer Register\n");
      return 1;
    }
    break;

  default:
    printf("[UART] Invalid port\n");
    return 1;
    break;
  }
  return 0;
}

int uart_send(uint8_t port, uint8_t data){
  uint8_t status;
  if(uart_get_status(port, &status) != 0){
    printf("[UART] Error getting status\n");
    return 1;
  }
  if((status & LSR_THRE) == 0){
    printf("[UART] Transmitter Holding Register Empty bit not set\n");
    return 1;
  }
  switch (port){
  case 1:
    if(sys_outb(COM1_UART_BASE + Tx_Holding_Reg, data) != 0){
      printf("[UART] Error writing to Tx Holding Register\n");
      return 1;
    }
    break;
  case 2:
    if(sys_outb(COM2_UART_BASE + Tx_Holding_Reg, data) != 0){
      printf("[UART] Error writing to Tx Holding Register\n");
      return 1;
    }
    break;

  default:
    printf("[UART] Invalid port\n");
    return 1;
    break;
  }
  return 0;
}


