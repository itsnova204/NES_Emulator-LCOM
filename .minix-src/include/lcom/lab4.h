#pragma once

#include <stdint.h>

struct packet {
  uint8_t bytes[3]; // mouse packet raw bytes
  bool rb, mb, lb;  // right, middle and left mouse buttons pressed
  int16_t delta_x;  // mouse x-displacement: rightwards is positive
  int16_t delta_y;  // mouse y-displacement: upwards is positive
  bool x_ov, y_ov;  // mouse x-displacement and y-displacement overflows
};

enum mouse_ev_t { LB_PRESSED,
                  LB_RELEASED,
                  RB_PRESSED,
                  RB_RELEASED,
                  BUTTON_EV,
                  MOUSE_MOV };

struct mouse_ev {
  enum mouse_ev_t type;
  int16_t delta_x, delta_y;
};

struct mouse_ev *(mouse_detect_event)(struct packet *pp);

/** 
 * @brief To test packet reception via interrupts 
 * 
 * Displays the packets received from the mouse
 * Exits after receiving the number of packets specified in argument
 * 
 * @param cnt Number of packets to receive and display before exiting
 * 
 * @return Return 0 upon success and non-zero otherwise
 */

int (mouse_test_packet)(uint32_t cnt);

/** 
 * @brief To test PS/2 remote mode operation 
 * 
 * Configures the PS/2 to operate in remote mode
 * 
 * Periodically requests a packet from the mouse,
 * and displays the packets received from the mouse
 * 
 * Exits after receiving the number of packets specified in argument
 * 
 * @param period Period in milliseconds 
 * @param cnt Number of packets to receive and display before exiting
 * 
 * @return Return 0 upon success and non-zero otherwise
 */

int (mouse_test_remote)(uint16_t period, uint8_t cnt);

/** 
 * @brief To test handling of more than one interrupt
 * 
 *  Similar test_packet() except that it 
 *  should terminate if no packets are received for idle_time seconds
 * 
 * @param idle_time Number of seconds without packets before exiting
 * 
 * @return Return 0 upon success and non-zero otherwise
 */

int (mouse_test_async)(uint8_t idle_time);

/** 
 * @brief To test state machine implementation
 * 
 *  Similar to mouse_test_packet() except that it 
 *  should terminate if a gesture is detected.
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance);

/** 
 * @brief Handles mouse interrupts
 * 
 * Reads the status register and the output buffer (OB).
 * 
 * If there was some error, the byte read from the OB should be discarded.
 * 
 * Must be defined using parenthesis around the function name:
 *
 *   void (mouse_ih)(void) {
 * 
 * @param IH's take no arguments
 * 
 * @return IH's take no values
 */

void (mouse_ih)(void);

/** 
 * @brief To print a mouse packet 
 * 
 * Displays a mouse packet in a human readable way
 * 
 * This function needs not be implemented: it is provided with the LCF
 * 
 * @param pp Pointer to a struct which contains both the packet bytes 
 * 				   and the parsed information.
 */

void (mouse_print_packet)(struct packet *pp);

/**
 * @brief Enables stream mode data reporting.
 *
 * Enables stream mode data reporting, by sending the respective command to the mouse.
 * 
 * This function is provided by the LCF.
 *
 * In order to score points for your Lab 4 grade, you should implement this functionality yourself. * This can be done by defining your own function with the same functionality, or just by adding
 * this functionality somewhere in your code.
 *
 * @return Returns O on success, and non-zero otherwise
 */

int (mouse_enable_data_reporting)(void);

/**
 * @brief Returns Minix's default KBC command byte.
 *
 * This function is already implemented by the LCF: you must not implement it.
 *
 * @return Returns Minix's default KBC command byte
 */

uint8_t (minix_get_dflt_kbc_cmd_byte)(void);
