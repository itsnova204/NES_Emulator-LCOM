/**
 * @file KBC.h
 * @brief This file contains the implementation of the KBC functions allowing handling of the keyboard and mouse interrupts
 * 
*/

#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"

/**
 * @brief Function to get the size of a scancode, can be 1 or 2
 * @param scancode scancode to get the size of
 * @return size of the scancode
*/
int(getScanCodeSize)(uint8_t scancode);

/**
 * @brief Function to check if the scan code is single byte or double byte
 * @param scancode scancode to check
 * @return true if the scancode is double byte, false otherwise
*/
bool(is_two_byte_scancode)(uint8_t scancode);

/**
 * @brief Function to print the scan code, mainly used for debugging
 * @param scancode scancode to print
 * @return 0 if successful, non 0 otherwise
*/
int print_scancode(uint8_t scancode);

/**
 * @brief Function to read the output buffer of the KBC
 * @param port port to read from
 * @param output pointer to the variable to store the output
 * @param mouse flag to indicate if the output is from the mouse or not (keyboard)
 * @return 0 if successful, non 0 otherwise
*/
int(kbc_read_output)(uint8_t port, uint8_t *output, bool mouse);

/**
 * @brief Function to write a command to the KBC
 * @param port port to write to
 * @param command command to write
 * @return 0 if successful, non 0 otherwise
*/
int(kbc_write_command)(uint8_t port, uint8_t command);

#endif
