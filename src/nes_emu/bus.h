/**
 * @file bus.h
 * @brief This file contains the implementation of the NES bus.
 * 
*/
#include "lcf_mock.h"
#pragma once


void bus_clock();
int bus_init(char* cart_filePath);
int bus_exit();

void sysBus_write(uint16_t addr, uint8_t data);
uint8_t sysBus_read(uint16_t addr);

union controler_t{
  //Dpad
  uint8_t up : 1;
  uint8_t left : 1;
  uint8_t down : 1;
  uint8_t right : 1;

  //menus
  uint8_t start : 1;
  uint8_t select : 1;

  //right buttons
  uint8_t a : 1;
  uint8_t b : 1;
};
