#include <lcom/lcf.h>
#pragma once

void ram_init();

void bus_write(uint16_t addr, uint8_t data);

uint8_t bus_read(uint16_t addr);
