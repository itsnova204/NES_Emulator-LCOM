#include <lcom/lcf.h>

uint8_t ram[64 * 1024];

void ram_init(){
  memset(&ram, 0, 64 * 1024);
}

void bus_write(uint16_t addr, uint8_t data){
  if (addr >= 0x0000 && addr <= 0xFFFF){ //access ram
    ram[addr] = data;
  }
  
}

uint8_t bus_read(uint16_t addr){
  if (addr >= 0x0000 && addr <= 0xFFFF){ //access ram
    return ram[addr];
  }

  return 0;
}
