/**
 * @file mouse.h
 * @brief This file contains the implementation of the mouse functions
 * 
*/

#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "kbc.h"
#include "i8042.h"

/**
 * @brief Subscribes the mouse interrupts
 * @param bit_no pointer to the variable to store the bit number
 * @return 0 if successful, non 0 otherwise
 * 
*/
int (mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes the mouse interrupts
 * @return 0 if successful, non 0 otherwise
*/
int (mouse_unsubscribe_int)();

/**
 * @brief The interrupt handler for the mouse
*/
void (mouse_int_handler)();

/**
 * @brief Syncs the mouse
 * @return True if the mouse is synced, false otherwise
*/
bool (mouse_sync)();

/**
 * @brief Parses the mouse packet
*/
void (mouse_parse_packet)();

/**
 * @brief Parses the mouse packet struct
 * @return 0 if successful, non 0 otherwise
*/
int (mouse_parse_struct)();

/**
 * @brief Writes a command to the mouse
 * @param cmd The command to write
 * @return 0 if successful, non 0 otherwise
*/
int (mouse_write_command)(uint8_t cmd);

/**
 * @brief Gets the mouse packet
 * @return The mouse packet
*/
struct packet (get_mouse_packet)();

# endif
