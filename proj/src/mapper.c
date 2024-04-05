#include "mapper.h"

static uint8_t mapper_id;
static uint16_t (*mapper_table[])(uint16_t) = {&mapper_000};

static uint8_t mapper_nPRGbanks;
static uint8_t mapper_nCHRbanks;

uint8_t set_mapper(uint8_t id, uint8_t nPRGbanks, uint8_t nCHRbanks){
  if (id >= sizeof(mapper_table) / sizeof(mapper_table[0])) return 1;
  mapper_id = id;
  mapper_nPRGbanks = nPRGbanks;
  mapper_nCHRbanks = nCHRbanks;
  return 0;
}

uint16_t mapper_map(uint16_t addr){
  return mapper_table[mapper_id](addr);
}

static uint16_t mapper_000(uint16_t addr){//TODO finish implementing this
  if (addr >= 0x8000 && addr <= 0xFFFF){
    return addr & (mapper_nPRGbanks > 1 ? 0x7FFF : 0x3FFF);
  }
  return addr;
}
