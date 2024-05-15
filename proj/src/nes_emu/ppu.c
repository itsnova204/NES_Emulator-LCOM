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



uint8_t fine_x = 0x00;
//Background rendering
uint16_t bg_shifter_pattern_lsb = 0x0000;
uint16_t bg_shifter_pattern_msb = 0x0000;
uint16_t bg_shifter_attrib_lsb  = 0x0000;
uint16_t bg_shifter_attrib_msb  = 0x0000;

uint8_t bg_next_tile_lsb 		= 0x00;
uint8_t bg_next_tile_msb 		= 0x00;
uint8_t bg_next_tile_attrib = 0x00;
uint8_t bg_next_tile_id     = 0x00;

bool nmi = false;
frame_complete = false;

//NES can only display 64 diferent colors 
uint8_t bytes_per_pixel = 6; 

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

	//256x240pixels
	ppu_out_buffer = malloc(256*240*bytes_per_pixel);
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
  return ppu_palette_ram[ppuBus_read(0x3F00 + (palette << 2) + pixel)];
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

//================================================================================================
//Redndering helpers

void IncrementScrollX(){
	if (ppu_regs.bg_enable || ppu_regs.sprite_enable){
		if (vram_addr.coarse_x == 31){

			// Leaving nametable so wrap address round
			vram_addr.coarse_x = 0;
			// Flip target nametable bit
			vram_addr.nametable_x = ~vram_addr.nametable_x;
		}else{
			// Staying in current nametable, so just increment
			vram_addr.coarse_x++;
		}
	}
};

void IncrementScrollY(){
		// Ony if rendering is enabled
		if (ppu_regs.bg_enable || ppu_regs.sprite_enable)
		{
			// If possible, just increment the fine y offset
			if (vram_addr.fine_y < 7){
				vram_addr.fine_y++;

			}else{
				// If we have gone beyond the height of a row, we need to
				// increment the row, potentially wrapping into neighbouring
				// vertical nametables. Dont forget however, the bottom two rows
				// do not contain tile information. The coarse y offset is used
				// to identify which row of the nametable we want, and the fine
				// y offset is the specific "scanline"

				// Reset fine y offset
				vram_addr.fine_y = 0;

				// Check if we need to swap vertical nametable targets
				if (vram_addr.coarse_y == 29)
				{
					// We do, so reset coarse y offset
					vram_addr.coarse_y = 0;
					// And flip the target nametable bit
					vram_addr.nametable_y = ~vram_addr.nametable_y;
				}
				else if (vram_addr.coarse_y == 31)
				{
					// In case the pointer is in the attribute memory, we
					// just wrap around the current nametable
					vram_addr.coarse_y = 0;
				}
				else
				{
					// None of the above boundary/wrapping conditions apply
					// so just increment the coarse y offset
					vram_addr.coarse_y++;
				}
			}
		} 
	};

	void TransferAddressX(){
		// Ony if rendering is enabled
		if (ppu_regs.bg_enable || ppu_regs.sprite_enable)
		{
			vram_addr.nametable_x = temp_addr.nametable_x;
			vram_addr.coarse_x    = temp_addr.coarse_x;
		}
	};

	void TransferAddressY(){
		// Ony if rendering is enabled
		if (ppu_regs.bg_enable || ppu_regs.sprite_enable)
		{
			vram_addr.fine_y      = temp_addr.fine_y;
			vram_addr.nametable_y = temp_addr.nametable_y;
			vram_addr.coarse_y    = temp_addr.coarse_y;
		}
	};

	void LoadBackgroundShifters(){	
		bg_shifter_pattern_lsb = (bg_shifter_pattern_lsb & 0xFF00) | bg_next_tile_lsb;
		bg_shifter_pattern_msb = (bg_shifter_pattern_lsb & 0xFF00) | bg_next_tile_msb;

		bg_shifter_attrib_lsb  = (bg_shifter_attrib_lsb & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
		bg_shifter_attrib_msb  = (bg_shifter_attrib_msb & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
	};

//================================================================================================
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
void ppu_clock(){ //TODO finish this (oh boy this is gonna be a journey) 

		if (scanline >= -1 && scanline < 240){		

			//Background rendering:

			if (scanline == 0 && ppu_cycle == 0){
				// "Odd Frame" cycle skip
				ppu_cycle = 1;
			}

			if (scanline == -1 && ppu_cycle == 1){
				ppu_regs.vertical_blank = 0;
			}

			if ((ppu_cycle >= 2 && ppu_cycle < 258) || (ppu_cycle >= 321 && ppu_cycle < 338)){
				UpdateShifters();

				switch ((ppu_cycle - 1) % 8)
				{
				case 0:
					LoadBackgroundShifters();


					bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));

					break;
				case 2:		
					bg_next_tile_attrib = ppuRead(0x23C0 | (vram_addr.nametable_y << 11) 
																						| (vram_addr.nametable_x << 10) 
																						| ((vram_addr.coarse_y >> 2) << 3) 
																						| (vram_addr.coarse_x >> 2));
					
			
					if (vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
					if (vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
					bg_next_tile_attrib &= 0x03;
					break;

				case 4: 

					bg_next_tile_lsb = ppuRead((ppu_regs.bg_pattern_table << 12) 
																	+ ((uint16_t)bg_next_tile_id << 4) 
																	+ (vram_addr.fine_y) + 0);

					break;
				case 6:

					bg_next_tile_msb = ppuRead((ppu_regs.bg_pattern_table << 12)
																	+ ((uint16_t)bg_next_tile_id << 4)
																	+ (vram_addr.fine_y) + 8);
					break;
				case 7:
					IncrementScrollX();
					break;
				}
			}
		
		// End of a visible scanline, so increment downwards...
		if (ppu_cycle == 256)
		{
			IncrementScrollY();
		}

		//...and reset the x position
		if (ppu_cycle == 257)
		{
			LoadBackgroundShifters();
			TransferAddressX();
		}

		// Superfluous reads of tile id at end of scanline
		if (ppu_cycle == 338 || ppu_cycle == 340)
		{
			bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
		}

		if (scanline == -1 && ppu_cycle >= 280 && ppu_cycle < 305)
		{
			// End of vertical blank period so reset the Y address ready for rendering
			TransferAddressY();
		}
	
	}


	if (scanline == 240){
		// Post Render Scanline - Do Nothing
	}

	if (scanline >= 241 && scanline < 261){
		if (scanline == 241 && ppu_cycle == 1){
			// Effectively end of frame, so set vertical blank flag
			ppu_regs.vertical_blank = 1;

			if (ppu_regs.NMI_enable == 1) 
				nmi = true;
		}
	}

	uint8_t background_pixel = 0;
	uint8_t background_palette = 0;

	if (ppu_regs.bg_enable = 1){ //TODO fix this!

		// Handle Pixel Selection by selecting the relevant bit
		// depending upon fine x scolling. This has the effect of
		// offsetting ALL background rendering by a set number
		// of pixels, permitting smooth scrolling
		uint16_t bit_mux = 0x8000 >> fine_x;

		// Select Plane pixels by extracting from the shifter 
		// at the required location. 
		uint8_t p0_pixel = (bg_shifter_pattern_lsb & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_msb & bit_mux) > 0;

		// Combine to form pixel index
		background_pixel = (p1_pixel << 1) | p0_pixel;

		// Get palette
		uint8_t background_palette_0 = (bg_shifter_attrib_lsb & bit_mux) > 0;
		uint8_t background_palette_1 = (bg_shifter_attrib_msb & bit_mux) > 0;
		background_palette = (background_palette_1 << 1) | background_palette_0;
	}


	//TODO: draw frame here



	//Simple PPU clock behaviour 
	ppu_cycle++;
	if (ppu_cycle >= 341)
	{
		ppu_cycle = 0;
		scanline++;
		if (scanline >= 261)
		{
			scanline = -1;
			frame_complete = true;
		}
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





void ppu_clock(){ //TODO finish this (oh boy this is gonna be a journey) 


		if (scanline >= -1 && scanline < 240){		

			//Background rendering:

			if (scanline == 0 && ppu_cycle == 0){
				// "Odd Frame" cycle skip
				ppu_cycle = 1;
			}

			if (scanline == -1 && ppu_cycle == 1){
				ppu_regs.vertical_blank = 0;
			}

			if ((ppu_cycle >= 2 && ppu_cycle < 258) || (ppu_cycle >= 321 && ppu_cycle < 338)){
				// Render Background
			}
			
					// End of a visible scanline, so increment downwards...
		if (cycle == 256)
		{
			IncrementScrollY();
		}

		//...and reset the x position
		if (cycle == 257)
		{
			LoadBackgroundShifters();
			TransferAddressX();
		}

		// Superfluous reads of tile id at end of scanline
		if (ppu_cycle == 338 || ppu_cycle == 340)
		{
			bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
		}

		if (scanline == -1 && ppu_cycle >= 280 && ppu_cycle < 305)
		{
			// End of vertical blank period so reset the Y address ready for rendering
			TransferAddressY();
		}

	}







	if (scanline == 240){
		// Post Render Scanline - Do Nothing
	}

	if (scanline >= 241 && scanline < 261){
		if (scanline == 241 && ppu_cycle == 1){
			// Effectively end of frame, so set vertical blank flag
			ppu_regs.vertical_blank = 1;

			if (ppu_regs.NMI_enable == 1) 
				nmi = true;
		}
	}

	uint8_t background_pixel = 0;
	uint8_t background_palette = 0;

	if (ppu_regs.bg_enable == 1){

		// Handle Pixel Selection by selecting the relevant bit
		// depending upon fine x scolling. This has the effect of
		// offsetting ALL background rendering by a set number
		// of pixels, permitting smooth scrolling
		uint16_t bit_mux = 0x8000 >> fine_x;

		// Select Plane pixels by extracting from the shifter 
		// at the required location. 
		uint8_t p0_pixel = (bg_shifter_pattern_lsb & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_msb & bit_mux) > 0;

		// Combine to form pixel index
		background_pixel = (p1_pixel << 1) | p0_pixel;

		// Get palette
		uint8_t background_palette_0 = (bg_shifter_attrib_lsb & bit_mux) > 0;
		uint8_t background_palette_1 = (bg_shifter_attrib_msb & bit_mux) > 0;
		background_palette = (background_palette_1 << 1) | background_palette_0;
	}


	//TODO: draw frame here




	//Simple PPU clock behaviour 
	ppu_cycle++;
	if (ppu_cycle >= 341)
	{
		ppu_cycle = 0;
		scanline++;
		if (scanline >= 261)
		{
			scanline = -1;
			frame_complete = true;
		}
	}
}

