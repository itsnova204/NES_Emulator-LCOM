#include "mapper.h"

static uint8_t mapper_id;
static uint16_t (*mapper_table[])(uint16_t, enum Type) = {&mapper_000};

static uint8_t mapper_nPRGbanks;
static uint8_t mapper_nCHRbanks;

uint8_t set_mapper(uint8_t id, uint8_t nPRGbanks, uint8_t nCHRbanks){
  if (id >= sizeof(mapper_table) / sizeof(mapper_table[0])) return 1;
  mapper_id = id;
  mapper_nPRGbanks = nPRGbanks;
  mapper_nCHRbanks = nCHRbanks;
  return 0;
}

uint16_t mapper_map(uint16_t addr, enum Type type){
  return mapper_table[mapper_id](addr, type);
}

uint16_t mapper_000(uint16_t addr, enum Type type){ //bounds are checked in the cartridge
  switch (type){
  case sys_read:
      return addr & (mapper_nPRGbanks > 1 ? 0x7FFF : 0x3FFF);
    break;
  case sys_write:
    return addr & (mapper_nPRGbanks > 1 ? 0x7FFF : 0x3FFF);
    break;
  case ppu_read:
      return addr;
      break;
  case ppu_write:
      return addr; 
      break; 
  default:
    break;
  }
  return 0;
}
