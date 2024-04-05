#include "lcf_mock.h"
#pragma once

uint8_t set_mapper(uint8_t id, uint8_t nPRGbanks, uint8_t nCHRbanks);
uint16_t mapper_map(uint16_t addr);

static uint16_t mapper_000(uint16_t addr);
