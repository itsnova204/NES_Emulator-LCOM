#include "lcf_mock.h"

#pragma once

void bus_init();

void mainBus_write(uint16_t addr, uint8_t data);
uint8_t mainBus_read(uint16_t addr);

void ppuBus_write(uint16_t addr, uint8_t data);
uint8_t ppuBus_read(uint16_t addr);

