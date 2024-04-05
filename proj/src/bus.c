#include "bus.h"

#include "p6502.h"
#include "ppu.h"
#include "cartridge.h"

static uint32_t mainClockCounter = 0;
uint8_t sys_ram[64 * 1024];

void bus_init(){
  //start sys_ram with 0;
  memset(&sys_ram, 0, 2048); //nes has 2kb of ram (0x0000 - 0x07FF)
}

void bus_clock(){
  ppu_clock();

  if (mainClockCounter % 3 == 0){
		cpu_clock();
	}

	mainClockCounter++;
}

void mainBus_write(uint16_t addr, uint8_t data){
  if (addr <= 0x1FFF){ //access sys_ram
    sys_ram[addr & 0x07FF] = data;
  }else if (addr >= 0x2000 && addr <= 0x3FFF){ //access ppu registers
    ppuBus_write(addr, data);
  }
}

uint8_t mainBus_read(uint16_t addr){
  if (addr <= 0x1FFF){ //access sys_ram
    return sys_ram[addr & 0x07FF];

  }else if (addr >= 0x2000 && addr <= 0x3FFF){ //access ppu registers
    return ppuBus_read(addr & 0x0007);

  }

  return 0;
}

void ppuBus_write(uint16_t addr, uint8_t data){

}

uint8_t ppuBus_read(uint16_t addr){

  return 0;
}
