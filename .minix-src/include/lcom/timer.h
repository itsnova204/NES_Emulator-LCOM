#ifndef __TIMER_H
#define __TIMER_H

#include <stdbool.h>
#include <stdint.h>

/** @defgroup timer timer
 * @{
 *
 * Functions for using the i8254 timers
 */

/**
 * @brief Enumerated type for specifying the timer value initialization
 */
enum timer_init {
  INVAL_val,    /*!< Invalid initialization mode */
  LSB_only,     /*!< Initialization only of the LSB */
  MSB_only,     /*!< Initialization only of the MSB */
  MSB_after_LSB /*!< Initialization of LSB and MSB, in this order */
};

/**
 * @brief Enumerated type for identifying the timer status fields
 */
enum timer_status_field {
  tsf_all,     /*!< configuration/status */
  tsf_initial, /*!< timer initialization mode */
  tsf_mode,    /*!< timer counting mode */
  tsf_base     /*!< timer counting base */
};

/**
 * @brief Union for storing values of timer status fields, including the full status byte
 */
union timer_status_field_val {
  uint8_t byte;            /*!< status */
  enum timer_init in_mode; /*!< initialization mode */
  uint8_t count_mode;      /*!< counting mode: 0, 1,.., 5 */
  bool bcd;                /*!< counting base, true if BCD */
};

/**
 * @brief Changes the operating frequency of a timer
 * 
 * Must use the read-back command so that it does not change 
 *   the 4 LSBs (mode and BCD/binary) of the timer's control word.
 * 
 * @param timer Timer to configure. (Ranges from 0 to 2)
 * @param freq Timer operating frequency
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_set_frequency)(uint8_t timer, uint32_t freq);

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_unsubscribe_int)();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void(timer_int_handler)();

/**
 * @brief Reads the input timer configuration (status) via read-back command
 *
 * @param timer Timer whose configuration to read (Ranges from 0 to 2)
 * @param st    Address of memory position to be filled with the timer config
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_get_conf)(uint8_t timer, uint8_t *st);

/**
 * @brief Shows timer configuration
 * 
 * Displays, in a human friendly way, the specified field of a 
 *  timer status, which was read via the read-back command
 *
 * @param timer timer whose configuration should be displayed (Ranges from 0 to 2)
 * @param st status read via the read-back command
 * @param field status field to display in human friendly way
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field);

/**
 * @brief Prints a timer config field value
 * 
 * @return Returns 0 upon success and non-zero otherwise 
 */
int(timer_print_config)(uint8_t timer, enum timer_status_field field,
                        union timer_status_field_val val);

/**
 * @brief Increments elapsed time count
 * 
 * @return Returns the current time count
 */
uint32_t(timer_print_elapsed_time)();

#endif /* __TIMER_H */
