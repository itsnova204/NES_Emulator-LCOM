#include "mapper.h"

static uint8_t mapper_id;
static uint16_t (*mapper_table[])(uint16_t, enum Type, bool*) = {&mapper_000};

static uint8_t mapper_nPRGbanks;
static uint8_t mapper_nCHRbanks;

uint8_t set_mapper(uint8_t id, uint8_t nPRGbanks, uint8_t nCHRbanks){
  printf("[mapper] loading mapper %d with: %d nPRGbanks, %d nCHRbanks\n",id, nPRGbanks,nCHRbanks);
  if (id >= sizeof(mapper_table) / sizeof(mapper_table[0])) return 1;
  mapper_id = id;
  mapper_nPRGbanks = nPRGbanks;
  mapper_nCHRbanks = nCHRbanks;
  return 0;
}

uint16_t mapper_map(uint16_t addr, enum Type type, bool* takeOver){
  return mapper_table[mapper_id](addr, type, takeOver);
}
int debugmapcounter = 0;
uint16_t mapper_000(uint16_t addr, enum Type type, bool* takeOver){ //bounds are checked in the cartridge
  *takeOver = false;

  switch (type){
  case type_sysBus_read:
      if (addr >= 0x8000){
        *takeOver = true;
        return addr & (mapper_nPRGbanks > 1 ? 0x7FFF : 0x3FFF);
      }
      return addr;
    break;
  case type_sysBus_write: //seems ok
    if (addr >= 0x8000){
      *takeOver = true;
    }
    
    return addr & (mapper_nPRGbanks > 1 ? 0x7FFF : 0x3FFF);
    break;
  case type_ppuBus_read_bus://seems ok
      if (addr <= 0x1FFF)
        {
          *takeOver = true;
          return addr;
        }

	    return addr;
      break;
  case type_ppuBus_write://seems ok
    if (addr <= 0x1FFF)
    {
      if (mapper_nCHRbanks == 0)
      {
        // Treat as RAM
        *takeOver = true;
        return addr;
      }
    }
    return addr; 
    break; 
  default:
    break;
  }
  return 0;
}

