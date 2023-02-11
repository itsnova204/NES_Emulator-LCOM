#pragma once

#include <stdbool.h>
#include <stdint.h>

/** @defgroup lab3 lab3
 * @{
 *
 * Functions for testing the kbd code
 */

/** 
 * @brief To test reading of scancodes via KBD interrupts 
 * 
 * Displays the scancodes received from the keyboard
 * Exits upon release of the ESC key
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_test_scan)();

/**
 * @brief To test reading of scancodes via polling
 * 
 * Displays the scancodes received from the keyboard
 * Exits upon release of the ESC key
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_test_poll)();

/** 
 * @brief To test handling of more than one interrupt
 * 
 *  Similar to kbd_test_scan() except that it 
 *  should terminate also if no scancodes are received for n seconds
 * 
 * @param n Number of seconds without scancodes before exiting
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_test_timed_scan)(uint8_t n);

/** 
 * @brief Interrupt handler to read scancodes in C.
 */
void (kbc_ih)(void);

/**
 * @brief To print the scancodes
 * 
 * Prints the scancodes via printf
 * Provided via the LCF -- no need to implement it
 * 
 * @param make Whether this is a make or a break code
 * @param size Size in bytes of the scancode
 * @param bytes Array with size elements, with the scancode bytes
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_print_scancode)(bool make, uint8_t size, uint8_t *bytes);

/**
 * @brief To print the no of sys_inb() calls
 * 
 * Prints the no of sys_inb() calls via printf
 * Provided via the LCF -- no need to implement it
 * 
 * @param no_calls Number of sys_inb() calls via printf
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_print_no_sysinb)(uint32_t cnt);
