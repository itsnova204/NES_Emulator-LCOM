/*=================================================================
=========================== rX2C0XX ppu ===========================
===================================================================
==Emulator for the PPU (picture processing unit) used in the NES ==
==DOCS: https://www.nesdev.org/wiki/PPU                          ==
==                                                               ==
== Tiago Aleixo, 2024                                            ==
=================================================================*/

//NOTE: scrolling is hard, here is a thread explaining implmentation
//https://forums.nesdev.org/viewtopic.php?t=664

#include "ppu.h"
#include "cartridge.h"

#include "bus.h"

uint8_t ppu_nametable_ram[2][1024]; //2kb of ram for nametables
uint8_t ppu_palette_ram[32]; //32 bytes of ram for palette
uint8_t ppu_patern_ram[2][4096]; //8192 bytes of ram for palette

//used for internal ppu comms
uint8_t ppu_address_latch = 0x00; 
uint8_t ppu_data_buffer = 0x00;
uint16_t ppu_address = 0x0000;

union ppu_registers ppu_regs;

vbe_mode_info_t vbe_mode_info;
void* ppu_out_buffer;

uint16_t scanline  = 0;
uint16_t ppu_cycle = 0;

void ppu_clock(){ //TODO finish this (oh boy this is gonna be a journey) 
	if (scanline >= -1 && scanline < 240){		
		if (scanline == 0 && ppu_cycle == 0){
			// "Odd Frame" cycle skip
			ppu_cycle = 1;
		}

		if (scanline == -1 && ppu_cycle == 1){
			ppu_regs.vertical_blank = 0;
		}
}}

int ppu_init(vbe_mode_info_t mode){//todo make vbe_mode_info updatable so it can change video mode during execution
	vbe_mode_info = mode;

	ppu_regs.PPU_CTRL_reg = 0;
	ppu_regs.PPU_MASK_reg = 0;
	ppu_regs.PPU_STATUS_reg = 0;
	ppu_regs.OAMADDR_reg = 0;
	ppu_regs.OAMDATA_reg = 0;
	ppu_regs.PPU_SCROLL_reg = 0;
	ppu_regs.PPU_ADDR_reg = 0;
	ppu_regs.PPU_DATA_reg = 0;

  memset(ppu_nametable_ram, 0, sizeof(ppu_nametable_ram));
	memset(ppu_palette_ram, 0, sizeof(ppu_palette_ram));
	memset(ppu_patern_ram, 0, sizeof(ppu_patern_ram));

	ppu_out_buffer = malloc(vbe_mode_info.XResolution * vbe_mode_info.YResolution * (vbe_mode_info.BitsPerPixel / 8));
	if (ppu_out_buffer == NULL){
		printf("ppu_init(): malloc(frambuffer) failed\n");
		return -1;
	}
	return 0;
}

void ppu_exit(){
	free(ppu_out_buffer);
}

uint8_t getColorFromPalette(uint8_t palette, uint8_t pixel){
  return ppu_palette_ram[(palette << 2) + pixel];
}

void getPatternTable(uint8_t patternTable, uint8_t palette){ //TODO finish this
  for (uint16_t cordY = 0; cordY < 16; cordY++){
	for (uint16_t cordX = 0; cordX < 16; cordX++){
		uint16_t offset = cordY * 256 + cordX * 16;

		for (uint16_t tileY = 0; tileY < 8; tileY++){
			uint8_t tile_lsb = ppuBus_read((patternTable * 0x1000) + offset + tileY);
			uint8_t tile_msb = ppuBus_read((patternTable * 0x1000) + offset + tileY + 8);

			for (uint16_t tileX = 0; tileX < 8; tileX++){
				uint8_t pixel = (tile_lsb & 0x01) + (tile_msb & 0x01);
				tile_lsb >>= 1;
				tile_msb >>= 1;


				uint8_t color = getColorFromPalette(palette, pixel);
	}
  }

}}}

//BUS IO:

//https://www.nesdev.org/wiki/PPU_registers
uint8_t sys_readFromPPU(uint16_t addr){//TODO finish this

	switch (addr){
	case PPU_CTRL: // Control NOT READABLE
		return 0;
		break;
	case PPU_MASK: // Mask NOT READABLE
		return 0;
		break;
	case PPU_STATUS: // Status
		ppu_address_latch = 0;
		ppu_regs.vertical_blank = 0;
		return ppu_regs.PPU_STATUS_reg & 0xE0; //only top 3 bits are relevant
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
		uint8_t data = ppu_data_buffer; //ppu reading is delayed by one read expet (read next)
		ppu_data_buffer = ppuBus_read(ppu_address);

		if (ppu_address >= 0x3F00){ //expet if it is beeing read from palette memory
			data = ppu_data_buffer;
		}
		
		break;
	}

	return 0;
}

//https://www.nesdev.org/wiki/PPU_registers
void sys_writeToPPU(uint16_t addr, uint8_t data){//TODO finish this
  
	switch (addr){
	case PPU_CTRL: // Control
		ppu_regs.PPU_CTRL_reg = data;
		//TODO temp ram adress
		break;
	case PPU_MASK: // Mask
		ppu_regs.PPU_MASK_reg = data;
		break;
	case PPU_STATUS: // Status
		break;
	case PPU_OAM_ADDRESS: // OAM Address
		break;
	case PPU_OAM_DATA: // OAM Data
		break;
	case PPU_SCROLL: // Scroll
		break;
	case PPU_ADDRESS: // PPU Address TODO: get this using loppy registers
		if (ppu_address_latch == 0){ //check if it is the first or second byte
			ppu_address_latch = 1;
			return (ppu_address & 0xFF00) ;
		}
		else{
			ppu_address_latch = 0;
			return ppu_address & 0x00FF;
		}
		break;
	case PPU_DATA: // PPU Data
		ppuBus_write(ppu_address, data);
		break;
	}
  
}


uint8_t ppuBus_read(uint16_t addr){ //DONE

	uint8_t data = 0x00;
	addr &= 0x3FFF;

	bool hijack = false;
	data = ppu_readFromCard(addr, &hijack);
	if (hijack == true){
		return data;
	}

  if (addr >= 0x2000 && addr <= 0x1FFF){//reading pattern table
		return ppu_patern_ram[(addr & 0x1000) >> 12][addr & 0x0FFF];
	}
	if (addr <= 0x3EFF){//reading nametable
		addr &= 0x0FFF; //mask for easier filtering
		enum MIRROR mirror = cart_get_mirror_type();


		if (mirror == VERTICAL)
		{
			// Vertical
			if (addr <= 0x03FF)
				return ppu_nametable_ram[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				return ppu_nametable_ram[1][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				return ppu_nametable_ram[0][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				return ppu_nametable_ram[1][addr & 0x03FF];
		}
		else if (mirror == HORIZONTAL)
		{
			// Horizontal
			if (addr <= 0x03FF)
				return ppu_nametable_ram[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				return ppu_nametable_ram[0][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				return ppu_nametable_ram[1][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				return ppu_nametable_ram[1][addr & 0x03FF];
		}
	}
  if (addr >= 0x3F00 && addr <= 0x3FFF){//writing palette
		addr &= 0x001F; //mask for easier filtering

		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;

		return ppu_palette_ram[addr] & (ppu_regs.greyscale ? 0x30 : 0x3F);
  }
  
  return 0;
}

void ppuBus_write(uint16_t addr, uint8_t data){//DONE
	addr &= 0x3FFF; 

	bool hijack = false;
	ppu_writeToCard(addr, data, &hijack);
	if (hijack == true){
		return;
	}
	
  if (addr <= 0x1FFF){//writing  pattern table
		ppu_patern_ram[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;

	}else if (addr >= 0x2000 && addr <= 0x3EFF){ //writing nametable

		addr &= 0x0FFF;
		enum MIRROR mirror = cart_get_mirror_type();

		if (mirror == VERTICAL)
		{
			// Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				ppu_nametable_ram[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				ppu_nametable_ram[1][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				ppu_nametable_ram[0][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				ppu_nametable_ram[1][addr & 0x03FF] = data;
		}
		else if (mirror == HORIZONTAL)
		{
			// Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				ppu_nametable_ram[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				ppu_nametable_ram[0][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				ppu_nametable_ram[1][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				ppu_nametable_ram[1][addr & 0x03FF] = data;
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF) //writing palette
	{
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		ppu_palette_ram[addr] = data;
	}
}