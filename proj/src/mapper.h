#include "lcf_mock.h"
#pragma once

enum Type{
  sys_read,
  sys_write,
  ppu_read,
  ppu_write,
};

uint8_t set_mapper(uint8_t id, uint8_t nPRGbanks, uint8_t nCHRbanks);
uint16_t mapper_map(uint16_t addr, enum Type type);

uint16_t mapper_000(uint16_t addr, enum Type type);