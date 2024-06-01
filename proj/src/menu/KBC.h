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
 * @brief Returns the size of the scancode
 * @param scancode The scancode to check the size
*/
int(getScanCodeSize)(uint8_t scancode);

/**
 * @brief Checks if the scancode is a two byte scancode
 * @param scancode The scancode to check
*/
bool(is_two_byte_scancode)(uint8_t scancode);

/**
 * @brief Prints the scancode
 * @param scancode The scancode to print
*/
int print_scancode(uint8_t scancode);

/**
 * @brief Reads a specific port
 * @param port The port to read from
 * @param output The data read from the port
 * @param mouse Boolean to check if it is a mouse scancode or a keyboard scancode
*/
int(kbc_read_output)(uint8_t port, uint8_t *output, bool mouse);

/**
 * @brief Writes a command to a specific port
 * @param port The port to write to
 * @param command The command to write
*/
int(kbc_write_command)(uint8_t port, uint8_t command);

#endif
