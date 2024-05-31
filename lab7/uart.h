/**
 * @file serialPort
 * @brief This file contains the functions that allows for the configuration and interruptions of the serial Port.
 *
 * @author Fábio Araújo de Sá, Inês Sá Pereira Estêvão Gaspar, Lourenço Alexandre Correia Gonçalves, Marcos William Ferreira Pinto
 *
 * @date 06/06/2022
 */

#ifndef _SERIAL_PORT_
#define _SERIAL_PORT_

#include <lcom/lcf.h>

#define SP_ATTEMPS 5

#ifndef __SP_H_
#define __SP_H_

#define COM1_UART_BASE 0x3F8
#define COM1_IRQ 4

#define RBR 0x00
#define THR  0x00
#define IER 0x01
#define IIR 0x02
#define FCR 0x02
#define LCR 0x03
#define MCR 0x04
#define LSR 0x05
#define MSR 0x06
#define SR	 0x07 	// Scratchpad Register (Read/Write)

#define DLL	 0x00  // Divisor Latch (Read/Write)                   (DLAB=1)
#define DLM	 0x01  // Divisor Latch (Read/Write)                   (DLAB=1)

#define LSR_DATA_READY BIT(0)
#define LSR_OE BIT(1)
#define LSR_PE BIT(2)
#define LSR_FE BIT(3)
#define LSR_BI BIT(4)
#define LSR_THRE BIT(5)
#define LSR_TRANS_EMPTY BIT(6)
#define LSR_RCVR_FIFO_ERROR BIT(7)

#define IER_ERBFI BIT(0)
#define IER_ETBEI BIT(1)
#define IER_ELSI BIT(2)
#define IER_EDSSI BIT(3)

#define IIR_NO_INT_PENDING BIT(0)
#define IIR_ID BIT(1) | BIT(2) | BIT(3)
#define IIR_OGN_LINE_STATUS BIT(1) | BIT(2)
#define IIR_OGN_RCVD_DATA_AVL BIT(2)
#define IIR_FIFO_CT BIT(3) | BIT(2)
#define IIR_OGN_TRANS_EMPTY BIT(1)
#define IIR_INT_PENDING 0x00

#define FCR_CLEAR BIT(0) | BIT(1) | BIT(2)

#endif

/**
* @brief This function subscribes serial port interrupts with policies REENABLE and EXCLUSIVE
* @param [out] bit_no All zeros except for the bit position of the serial port's hook id
* @return Return 0 if case of success, 1 if otherwise
*/
int sp_subscribe_int(uint8_t* bitno);

/**
* @brief This function unsubscribes serial port interrupts
* @return Return 0 if case of success, 1 if otherwise
*/
int sp_unsubscribe_int();

/**
* @brief This function reads the status of the serial port
* @param [out] status The current status of the serial port
* @return Return 0 if case of success, 1 if otherwise
*/
int sp_get_status(uint8_t* status);

/**
* @brief This function initializes the serial port, enabling interrupts and creating the received data queue
*/
void sp_init();

/**
* @brief Serial ports's interrupt handler
*/
void sp_ih();

int sp_get_ctrl(uint8_t* status);

/**
* @brief This function sends a byte through the serial port
* @param [out] byte The byte to send
* @return Return 0 if case of success, 1 if otherwise
*/
int send_byte(uint8_t byte);

/**
* @brief This function reads a byte from the serial port and pushes it to the received data queue
* @return Return 0 if case of success, 1 if otherwise
*/
int read_byte();

/**
* @brief This function is used to get the received data queue
* @return Pointer to the received data queue
*/


/**
 * @brief The function prepares the queue and FCR for the next interruptiom, it empties the queue to prevent errors in the next use
 * @return Return 0 if case of success, 1 if otherwise
 */
int SP_clearInterrupts();


#endif // _SERIAL_PORT_
