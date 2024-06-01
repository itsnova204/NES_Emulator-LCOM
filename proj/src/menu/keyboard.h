/**
 * @file keyboard.h
 * @brief This file contains the implementation of the keyboard functions
 * 
*/

#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"
#include "KBC.h"

/**
 * @brief Function to get the scan code
 * @return scan code
*/
uint8_t(get_scancode)();

/**
 * @brief Checks if the scancode read on kbc_ih is valid
 * @return True if the scancode is valid, false otherwise
*/
bool(is_valid)();

/**
 * @brief Function to subscribe the keyboard interrupts
 * @param bit_no pointer to the variable to store the bit number
 * @return 0 if successful, non 0 otherwise
*/
int(kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Function to unsubscribe the keyboard interrupts
 * @return 0 if successful, non 0 otherwise
*/
int(kbd_unsubscribe_int)();

/**
 * @brief Function to handle the keyboard interrupts
*/
void(kbc_ih)();

/**
 * @brief Function to restore the keyboard state
 * @return 0 if successful, non 0 otherwise
*/
int(kbd_interrupt_enabler)();

#endif
