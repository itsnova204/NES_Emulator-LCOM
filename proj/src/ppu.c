/*=================================================================
=========================== rX2C0XX ppu ===========================
===================================================================
==Emulator for the PPU (picture processing unit) used in the NES ==
==DOCS: https://www.nesdev.org/wiki/PPU                          ==
==                                                               ==
== Tiago Aleixo, 2024                                            ==
=================================================================*/
#include "ppu.h"

#include "bus.h"

uint8_t ppu_nametable_ram[2][1024]; //2kb of ram for nametables
uint8_t ppu_palette_ram[32]; //32 bytes of ram for palette
uint8_t ppu_patern_ram[2][4096]; //8192 bytes of ram for palette

union ppu_registers ppu_regs;


void ppu_clock(){
  
}

//https://www.nesdev.org/wiki/PPU_registers
uint8_t sys_readFromPPU(uint16_t addr){

	switch (addr){
	case PPU_CTRL: // Control
		break;
	case PPU_MASK: // Mask
		break;
	case PPU_STATUS: // Status
		break;
	case PPU_OAM_ADDRESS: // OAM Address
		break;
	case PPU_OAM_DATA: // OAM Data
		break;
	case PPU_SCROLL: // Scroll
		break;
	case PPU_ADDRESS: // PPU Address
		break;
	case PPU_DATA: // PPU Data
		break;
	}

	return 0;
}

//https://www.nesdev.org/wiki/PPU_registers
void sys_writeToPPU(uint16_t addr, uint8_t data){
  
	switch (addr){
	case PPU_CTRL: // Control
		break;
	case PPU_MASK: // Mask
		break;
	case PPU_STATUS: // Status
		break;
	case PPU_OAM_ADDRESS: // OAM Address
		break;
	case PPU_OAM_DATA: // OAM Data
		break;
	case PPU_SCROLL: // Scroll
		break;
	case PPU_ADDRESS: // PPU Address
		break;
	case PPU_DATA: // PPU Data
		break;
	}
  
}

uint8_t ppuBus_read(uint16_t addr){
  if (addr <= 0x1FFF){
    return ppu_readFromCard(addr);
  }
  if (addr >= 0x2000 && addr <= 0x1FFF){//writing  pattern table
		return ppu_patern_ram[(addr & 0x1000) >> 12][addr & 0x0FFF];
	}
  if (addr <= 0x3EFF){//writing nametable
    //TODO: implement nametable writing
  }
  if (addr >= 0x3F00 && addr <= 0x3FFF){//writing palette
    //TODO: implement palette writing
  }
  
  return 0;
}

void ppuBus_write(uint16_t addr, uint8_t data){
  if (addr <= 0x1FFF){
    ppu_writeToCard(addr, data);
  }
  if (addr >= 0x2000 && addr <= 0x1FFF){//writing  pattern table
		data = ppu_patern_ram[(addr & 0x1000) >> 12][addr & 0x0FFF];
	}
  if (addr <= 0x3EFF){//writing nametable
    //TODO: implement nametable writing
  }
  if (addr >= 0x3F00 && addr <= 0x3FFF){//writing palette
    //TODO: implement palette writing
  }
}