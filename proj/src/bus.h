#include "lcf_mock.h"

#pragma once

void bus_init();

void sysBus_write(uint16_t addr, uint8_t data);
uint8_t sysBus_read(uint16_t addr);

void ppuBus_write(uint16_t addr, uint8_t data);
uint8_t ppuBus_read_bus(uint16_t addr);

