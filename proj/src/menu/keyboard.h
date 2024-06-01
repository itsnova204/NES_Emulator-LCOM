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
 * @brief Gets the scancode
 * @return The scancode
*/
uint8_t(get_scancode)();

/**
 * @brief Checks if the scancode read on kbc_ih is valid
 * @return True if the scancode is valid, false otherwise
*/
bool(is_valid)();

/**
 * @brief Subscribes the keyboard interrupts
 * @param bit_no The bit number to set in the mask
*/
int(kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes the keyboard interrupts
*/
int(kbd_unsubscribe_int)();

/**
 * @brief The interrupt handler for the keyboard
*/
void(kbc_ih)();

/**
 * @brief Enables the interrupts for the keyboard
*/
int(kbd_interrupt_enabler)();

#endif
