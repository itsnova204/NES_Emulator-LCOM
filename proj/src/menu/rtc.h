/**
 * @file rtc.h
 * @brief This file contains the implementation of the RTC functions  
 * 
*/

#ifndef _LCOM_RTC_H_
#define _LCOM_RTC_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>

#define RTC_IRQ 8

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define RTC_SECONDS_LOC 0
#define RTC_MINUTES_LOC 2
#define RTC_HOURS_LOC 4
#define RTC_DAY_LOC 7
#define RTC_MONTH_LOC 8
#define RTC_YEAR_LOC 9

#define RTC_SECONDS_ALARM_LOC 1
#define RTC_MINUTES_ALARM_LOC 3
#define RTC_HOURS_ALARM_LOC 5

#define RTC_REG_A 10
#define RTC_REG_B 11
#define RTC_REG_C 12
#define RTC_REG_D 13

#define UIP_REG_A BIT(7)

#define RTC_DONT_CARE (BIT(7) | BIT(6))

#define SET_REG_B BIT(7)
#define PIE_REG_B BIT(6)
#define AIE_REG_B BIT(5)
#define UIE_REG_B BIT(4)

#define AF_REG_C BIT(5)
#define UF_REG_C BIT(4)

/**
 * @struct rtc_date_t
 * @brief Struct that contains the date and time
*/
typedef struct {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t day;
  uint8_t month;
  uint8_t year;
} rtc_date_t;

/**
 * @brief Subscribes and enables RTC interrupts
*/
int (rtc_subscribe_int)(uint16_t *bit_no);

/**
 * @brief Unsubscribes RTC interrupts
*/
int (rtc_unsubscribe_int)();

/**
 * @brief Reads the date from the RTC
*/
int (rtc_read_date)();

/**
 * @brief Reads a register from the RTC
 * @param reg Register to read
 * @param data Pointer to the variable that will store the data read
*/
int (rtc_read_reg)(uint8_t reg, uint8_t *data);

/**
 * @brief Writes a register from the RTC
 * @param reg Register to write
 * @param data Data to write
*/
int (rtc_write_reg)(uint8_t reg, uint8_t data);

/**
 * @brief Converts a binary number to BCD
 * @param binary Binary number to convert
 * @return Converted BCD number
*/
uint8_t (rtc_bcd_to_binary)(uint8_t bcd);

/**
 * @brief RTC interrupt handler
*/
void (rtc_ih)();

/**
 * @brief Enables RTC alarm interrupt
*/
int (rtc_enable_alarm_int)();

/**
 * @brief Disables RTC alarm interrupt
*/
int (rtc_disable_int)();

/**
 * @brief Gets the date from the RTC
 * @return rtc_date_t struct with the date
*/
rtc_date_t (rtc_get_date)();

/**
 * @brieg Sets bit 7 of register B to 1
*/
int (rtc_set_regB_bit)();

/**
 * @brief Clears bit 7 of register B
*/
int (rtc_clear_regB_bit)();

/**
 * @brief Sets the alarm of the RTC
*/
int (rtc_set_alarm)();

#endif
