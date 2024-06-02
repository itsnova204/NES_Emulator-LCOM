#include "lcf_mock.h"

/**
 * @file mapper.h
 * @brief This file contains the implementation of NES mappers.
 * @brief https://www.nesdev.org/wiki/Mapper
*/


#pragma once

enum Type{
  type_sysBus_read,
  type_sysBus_write,
  type_ppuBus_read_bus,
  type_ppuBus_write,
};

uint8_t set_mapper(uint8_t id, uint8_t nPRGbanks, uint8_t nCHRbanks);
uint16_t mapper_map(uint16_t addr, enum Type type, bool* hijack);

uint16_t mapper_000(uint16_t addr, enum Type type, bool* hijack);
