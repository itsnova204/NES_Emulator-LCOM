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


Color ColorBuild(uint8_t red, uint8_t green, uint8_t blue) {
    return (Color){red, green, blue, 255};
}

static Ppu2C02 ppu;

bool ppu_isFrameComplete() {
    return ppu.frameCompleted;
}

void ppu_setFrameCompleted(bool value) {
    ppu.frameCompleted = value;
}

void ppu_disable_nmi(){
    ppu.nmi = false;
}

//============================================
//PPU FORGROUND


struct sObjectAttributeEntry OAM[64];
uint8_t* pOAM = (uint8_t*)OAM;

uint8_t* getOAM_ptr(){
    return pOAM;
}

uint8_t oam_addr = 0x00;
struct sObjectAttributeEntry spriteScanline[8];

uint8_t sprite_count = 0;
uint8_t sprite_shifter_pattern_lo[9];
uint8_t sprite_shifter_pattern_hi[9];

// Sprite Zero Collision Flags
bool bSpriteZeroHitPossible = false;
bool bSpriteZeroBeingRendered = false;
//============================================

int debugcount = 0;

Sprite* ppu_get_screen_ptr() {
		return ppu.spriteScreen;
}

void ppu_init() {
    printf("[PPU] Initializing PPU\n");

    ppu.paletteScreen[0x00] = ColorBuild(84, 84, 84);
    ppu.paletteScreen[0x01] = ColorBuild(0, 30, 116);
    ppu.paletteScreen[0x02] = ColorBuild(8, 16, 144);
    ppu.paletteScreen[0x03] = ColorBuild(48, 0, 136);
    ppu.paletteScreen[0x04] = ColorBuild(68, 0, 100);
    ppu.paletteScreen[0x05] = ColorBuild(92, 0, 48);
    ppu.paletteScreen[0x06] = ColorBuild(84, 4, 0);
    ppu.paletteScreen[0x07] = ColorBuild(60, 24, 0);
    ppu.paletteScreen[0x08] = ColorBuild(32, 42, 0);
    ppu.paletteScreen[0x09] = ColorBuild(8, 58, 0);
    ppu.paletteScreen[0x0A] = ColorBuild(0, 64, 0);
    ppu.paletteScreen[0x0B] = ColorBuild(0, 60, 0);
    ppu.paletteScreen[0x0C] = ColorBuild(0, 50, 60);
    ppu.paletteScreen[0x0D] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x0E] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x0F] = ColorBuild(0, 0, 0);

    ppu.paletteScreen[0x10] = ColorBuild(152, 150, 152);
    ppu.paletteScreen[0x11] = ColorBuild(8, 76, 196);
    ppu.paletteScreen[0x12] = ColorBuild(48, 50, 236);//looking for this color
    ppu.paletteScreen[0x13] = ColorBuild(92, 30, 228);
    ppu.paletteScreen[0x14] = ColorBuild(136, 20, 176);
    ppu.paletteScreen[0x15] = ColorBuild(160, 20, 100);
    ppu.paletteScreen[0x16] = ColorBuild(152, 34, 32);
    ppu.paletteScreen[0x17] = ColorBuild(120, 60, 0);
    ppu.paletteScreen[0x18] = ColorBuild(84, 90, 0);
    ppu.paletteScreen[0x19] = ColorBuild(40, 114, 0);
    ppu.paletteScreen[0x1A] = ColorBuild(8, 124, 0);
    ppu.paletteScreen[0x1B] = ColorBuild(0, 118, 40);
    ppu.paletteScreen[0x1C] = ColorBuild(0, 102, 120);
    ppu.paletteScreen[0x1D] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x1E] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x1F] = ColorBuild(0, 0, 0);

    ppu.paletteScreen[0x20] = ColorBuild(236, 238, 236);
    ppu.paletteScreen[0x21] = ColorBuild(76, 154, 236);
    ppu.paletteScreen[0x22] = ColorBuild(120, 124, 236);
    ppu.paletteScreen[0x23] = ColorBuild(176, 98, 236);
    ppu.paletteScreen[0x24] = ColorBuild(228, 84, 236);
    ppu.paletteScreen[0x25] = ColorBuild(236, 88, 180);
    ppu.paletteScreen[0x26] = ColorBuild(236, 106, 100);
    ppu.paletteScreen[0x27] = ColorBuild(212, 136, 32);
    ppu.paletteScreen[0x28] = ColorBuild(160, 170, 0);
    ppu.paletteScreen[0x29] = ColorBuild(116, 196, 0);
    ppu.paletteScreen[0x2A] = ColorBuild(76, 208, 32);
    ppu.paletteScreen[0x2B] = ColorBuild(56, 204, 108);
    ppu.paletteScreen[0x2C] = ColorBuild(56, 180, 204);
    ppu.paletteScreen[0x2D] = ColorBuild(60, 60, 60);
    ppu.paletteScreen[0x2E] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x2F] = ColorBuild(0, 0, 0);

    ppu.paletteScreen[0x30] = ColorBuild(236, 238, 236);
    ppu.paletteScreen[0x31] = ColorBuild(168, 204, 236);
    ppu.paletteScreen[0x32] = ColorBuild(188, 188, 236);
    ppu.paletteScreen[0x33] = ColorBuild(212, 178, 236);
    ppu.paletteScreen[0x34] = ColorBuild(236, 174, 236);
    ppu.paletteScreen[0x35] = ColorBuild(236, 174, 212);
    ppu.paletteScreen[0x36] = ColorBuild(236, 180, 176);
    ppu.paletteScreen[0x37] = ColorBuild(228, 196, 144);
    ppu.paletteScreen[0x38] = ColorBuild(204, 210, 120);
    ppu.paletteScreen[0x39] = ColorBuild(180, 222, 120);
    ppu.paletteScreen[0x3A] = ColorBuild(168, 226, 144);
    ppu.paletteScreen[0x3B] = ColorBuild(152, 226, 180);
    ppu.paletteScreen[0x3C] = ColorBuild(160, 214, 228);
    ppu.paletteScreen[0x3D] = ColorBuild(160, 162, 160);
    ppu.paletteScreen[0x3E] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x3F] = ColorBuild(0, 0, 0);

    ppu.spriteScreen = SpriteCreate(256, 240);

	
    ppu.spriteNameTable[0] = SpriteCreate(256, 240);
    ppu.spriteNameTable[1] = SpriteCreate(256, 240);
    ppu.spritePatternTable[0] = SpriteCreate(128, 128);
    ppu.spritePatternTable[1] = SpriteCreate(128, 128);
    ppu.scanline = 0;
    ppu.cycle = 0;
    ppu.frameCompleted = false;

    ppu.registers.ctrl.reg = 0;
    ppu.registers.status.reg = 0;
    ppu.registers.mask.reg = 0;

    ppu.addressLatch = 0x00;
    ppu.ppuDataBuffer = 0x00;
    ppu.vramAddr.reg = 0x0000;
    ppu.tramAddr.reg = 0x0000;
    ppu.fineX = 0x00;

    ppu.bgNextTileId = 0x00;
    ppu.bgNextTileAttr = 0x00;
    ppu.bgNextTileLsb = 0x00;
    ppu.bgNextTileMsb = 0x00;

    ppu.bgShifterPatternLo = 0x0000;
    ppu.bgShifterPatternHi = 0x0000;
    ppu.bgShifterAttribLo = 0x0000;
    ppu.bgShifterAttribHi = 0x0000;

    ppu.nmi = false;
	printf("[PPU] PPU finished initializing\n");
}

void ppu_exit(){
	free(ppu.spriteScreen->pixels);
	free(ppu.spriteScreen);
	free(ppu.spriteNameTable[0]->pixels);
	free(ppu.spriteNameTable[0]);
	free(ppu.spriteNameTable[1]->pixels);
	free(ppu.spriteNameTable[1]);
	free(ppu.spritePatternTable[0]->pixels);
	free(ppu.spritePatternTable[0]);
	free(ppu.spritePatternTable[1]->pixels);
	free(ppu.spritePatternTable[1]);
}

Sprite *SpriteCreate(uint16_t width, uint16_t height) {
    Sprite *sprite = (Sprite*)malloc(sizeof(uint16_t)*2+sizeof(Color*));
    sprite->width = width;
    sprite->height = height;
    sprite->pixels = (Color*)malloc(width*height*sizeof(Color));

	memset(sprite->pixels, 0, width*height*sizeof(Color));
    return sprite;
}

Color SpriteGetPixel(Sprite *sprite, uint16_t x, uint16_t y) {
    return sprite->pixels[x*sprite->height + y];
}

bool SpriteSetPixel(Sprite *sprite, uint16_t x, uint16_t y, Color color) {
    if (x < sprite->width && y < sprite->height) {
        sprite->pixels[x*sprite->height + y] = color;
        return true;
    }
    return false;
}

uint8_t cpuBus_readPPU(uint16_t addr) { //known good
    uint8_t data = 0x00;

        switch (addr) {
		
            // Control - Not readable
            case 0x0000: break;
            
            // Mask - Not Readable
            case 0x0001: break;
            
            // Status
            case 0x0002:
                data = (ppu.registers.status.reg & 0xE0) | (ppu.ppuDataBuffer & 0x1F);
                ppu.registers.status.bits.verticalBlank = 0;
                ppu.addressLatch = 0;
                break;

            // OAM Address
            case 0x0003: break;
            // OAM Data
            case 0x0004: break;
                data = pOAM[oam_addr];

            // Scroll - Not Readable
            case 0x0005: break;

            // PPU Address - Not Readable
            case 0x0006: break;

            // PPU Data
            case 0x0007: 
                data = ppu.ppuDataBuffer;
                ppu.ppuDataBuffer = ppuBus_read(ppu.vramAddr.reg);
                if (ppu.vramAddr.reg >= 0x3F00) data = ppu.ppuDataBuffer;
                ppu.vramAddr.reg += ppu.registers.ctrl.bits.incrementMode ? 32 : 1;
                break;
		}
	
    return data;
}

void cpuBus_writePPU(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x0000: // Control
		ppu.registers.ctrl.reg = data;
		ppu.tramAddr.bits.nametableX = ppu.registers.ctrl.bits.nametableX;
		ppu.tramAddr.bits.nametableY = ppu.registers.ctrl.bits.nametableY;
		break;
	case 0x0001: // Mask
		ppu.registers.mask.reg = data;
		break;
	case 0x0002: // Status
		break;
	case 0x0003: // OAM Address
        oam_addr = data;
		break;
	case 0x0004: // OAM Data
        pOAM[oam_addr] = data;
		break;
	case 0x0005: // Scroll
		if (ppu.addressLatch == 0) {
			ppu.fineX = data & 0x07;
			ppu.tramAddr.bits.coarseX = data >> 3;
			ppu.addressLatch = 1;
		}
		else {
			ppu.tramAddr.bits.fineY = data & 0x07;
			ppu.tramAddr.bits.coarseY = data >> 3;
			ppu.addressLatch = 0;
		}
		break;
	case 0x0006: // PPU Address
            if (ppu.addressLatch == 0) {
                ppu.tramAddr.reg = (uint16_t)((data & 0x3F) << 8) | (ppu.tramAddr.reg & 0x00FF);
                ppu.addressLatch = 1;
            }
            else {
                ppu.tramAddr.reg = (ppu.tramAddr.reg & 0xFF00) | data;
                ppu.vramAddr = ppu.tramAddr; 
                ppu.addressLatch = 0;
            }
            break;
	case 0x0007: // PPU Data
            ppuBus_write(ppu.vramAddr.reg, data);
            ppu.vramAddr.reg += ppu.registers.ctrl.bits.incrementMode ? 32 : 1;
		break;
	}
	
}

uint8_t ppuBus_read(uint16_t addr) {

    uint8_t data = 0x00;
    addr &= 0x3FFF;
    bool hijack = false;	
    data = ppu_readFromCard(addr, &hijack);
    if (hijack) return data;
    // Pattern table
    else if (addr <= 0x1FFF) {
        // Get the most significant bit of the address and offsets by the rest of the bits of the address
        data = ppu.patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF];
        
    }
    // Name table
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        enum MIRROR mirror = cart_get_mirror_type();
        
        if (mirror == VERTICAL) {
            if (addr <= 0x03FF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0400 && addr <= 0x07FF) data = ppu.nameTable[1][addr & 0x03FF];
            if (addr >= 0x0800 && addr <= 0x0BFF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0C00 && addr <= 0x0FFF) data = ppu.nameTable[1][addr & 0x03FF];
        }
        else if (mirror == HORIZONTAL) {
            if (addr <= 0x03FF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0400 && addr <= 0x07FF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0800 && addr <= 0x0BFF) data = ppu.nameTable[1][addr & 0x03FF];
            if (addr >= 0x0C00 && addr <= 0x0FFF) data = ppu.nameTable[1][addr & 0x03FF];
        }
        
        //printf("Reading from name table %d addr %02x data %01x\n", debugcount++,addr,data);

        
    }
    // Palette
    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		data = ppu.paletteTable[addr] & (ppu.registers.mask.bits.grayscale ? 0x30 : 0x3F);
        
    }

    return data;
}

void ppuBus_write(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;
    bool hijack;	
	//Writing to PPU addr 2400 data 24

	//printf("Writing to PPU addr");
    ppu_writeToCard(addr, data, &hijack);
    
	if (hijack) return;
    // Pattern table
    else if (addr <= 0x1FFF) {
        // This memory acts as a ROM for the PPU,
        // but for som NES ROMs, this memory is a RAM.
        ppu.patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    }
    // Name table
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        enum MIRROR mirror = cart_get_mirror_type();
        if (mirror == VERTICAL) {
   
            if (addr <= 0x03FF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0400 && addr <= 0x07FF) ppu.nameTable[1][addr & 0x03FF] = data;
            if (addr >= 0x0800 && addr <= 0x0BFF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0C00 && addr <= 0x0FFF) ppu.nameTable[1][addr & 0x03FF] = data;
   
        }
        else if (mirror == HORIZONTAL) {
            if (addr <= 0x03FF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0400 && addr <= 0x07FF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0800 && addr <= 0x0BFF) ppu.nameTable[1][addr & 0x03FF] = data;
            if (addr >= 0x0C00 && addr <= 0x0FFF) ppu.nameTable[1][addr & 0x03FF] = data;
        }
        if (data != 0)
        {
            //printf("%d found data: %02x\n",debugcount++, data);
        }
        
    }
    // Palette
    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        //printf("Writing to palette, addr=%04x, data=%02x\n", addr, data);
        addr &= 0x001F; // Mask less significant 5 bits
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        ppu.paletteTable[addr] = data;//KNOWN GOOD!
        
    }
	

}

Color get_colorFromPaletteRam(uint8_t palette, uint8_t pixel) {//KNOWN GOOD!
    Color color = ppu.paletteScreen[ppuBus_read(0x3F00 + (palette << 2) + pixel) & 0x3F];    
    return color;
}

Sprite *get_patternTable(uint8_t i, uint8_t palette) {
    for (uint16_t nTileY = 0; nTileY < 16; nTileY++) {
        for (uint16_t nTileX = 0; nTileX < 16; nTileX++) {
            uint16_t nOffset = nTileY * 256 + nTileX * 16;
            // Now loop through 8 rows of 8 pixels (Tile)
            for (uint16_t row = 0; row < 8; row++) {
                uint8_t tile_lsb = ppuBus_read(i * 0x1000 + nOffset + row + 0x0000);
			    uint8_t tile_msb = ppuBus_read(i * 0x1000 + nOffset + row + 0x0008);
                for (uint16_t col = 0; col < 8; col++) {
                    uint8_t pixel = (tile_lsb & 0x01) + (tile_msb & 0x01);
                    tile_lsb >>= 1; tile_msb >>= 1;

                    Color c = get_colorFromPaletteRam(palette, pixel);

                    
                    SpriteSetPixel(ppu.spritePatternTable[i], nTileX * 8 + (7 - col), nTileY * 8 + row, c);
                }
            }
        }
    }
    return ppu.spritePatternTable[i];
}

void IncrementScrollX() {
    if (ppu.registers.mask.bits.renderBackground || ppu.registers.mask.bits.renderSprites) {
        if (ppu.vramAddr.bits.coarseX == 31) {
            ppu.vramAddr.bits.coarseX = 0;
            ppu.vramAddr.bits.nametableX = ~ppu.vramAddr.bits.nametableX;
        }
        else {
            ppu.vramAddr.bits.coarseX++;
        }
    }
}

void IncrementScrollY() {
    if (ppu.registers.mask.bits.renderBackground || ppu.registers.mask.bits.renderSprites) {
        if (ppu.vramAddr.bits.fineY < 7) {
            ppu.vramAddr.bits.fineY++;
        }
        else {
            ppu.vramAddr.bits.fineY = 0;
            if (ppu.vramAddr.bits.coarseY == 29) {
                ppu.vramAddr.bits.coarseY = 0;
                ppu.vramAddr.bits.nametableY = ~ppu.vramAddr.bits.nametableY;
            }
            else if (ppu.vramAddr.bits.coarseY == 31) {
                ppu.vramAddr.bits.coarseY = 0;
            }
            else {
                ppu.vramAddr.bits.coarseY++;
            }
        }
    }
}

void TransferAddressX() {
    if (ppu.registers.mask.bits.renderBackground || ppu.registers.mask.bits.renderSprites) {
        ppu.vramAddr.bits.nametableX = ppu.tramAddr.bits.nametableX;
        ppu.vramAddr.bits.coarseX = ppu.tramAddr.bits.coarseX;
    }
}

void TransferAddressY() {
    if (ppu.registers.mask.bits.renderBackground || ppu.registers.mask.bits.renderSprites) {
        ppu.vramAddr.bits.fineY = ppu.tramAddr.bits.fineY;
        ppu.vramAddr.bits.nametableY = ppu.tramAddr.bits.nametableY;
        ppu.vramAddr.bits.coarseY = ppu.tramAddr.bits.coarseY;
    }
}

void LoadBackgroundShifters() {
    ppu.bgShifterPatternLo = (ppu.bgShifterPatternLo & 0xFF00) | ppu.bgNextTileLsb;
    ppu.bgShifterPatternHi = (ppu.bgShifterPatternHi & 0xFF00) | ppu.bgNextTileMsb;
    ppu.bgShifterAttribLo = (ppu.bgShifterAttribLo & 0xFF00) | ((ppu.bgNextTileAttr & 1) ? 0xFF : 0x00);
    ppu.bgShifterAttribHi = (ppu.bgShifterAttribHi & 0xFF00) | ((ppu.bgNextTileAttr & 2) ? 0xFF : 0x00);
}

void UpdateShifters() {
    if (ppu.registers.mask.bits.renderBackground) {
        ppu.bgShifterPatternLo <<= 1;
        ppu.bgShifterPatternHi <<= 1;
        ppu.bgShifterAttribLo <<= 1;
        ppu.bgShifterAttribHi <<= 1;
    }

    if (ppu.registers.mask.bits.renderSprites && ppu.cycle >= 1 && ppu.cycle < 258)
	{
		for (int i = 0; i < sprite_count; i++)
		{
			if (spriteScanline[i].x > 0)
			{
				spriteScanline[i].x--;
			}
			else
			{
				sprite_shifter_pattern_lo[i] <<= 1;
				sprite_shifter_pattern_hi[i] <<= 1;
			}
		}
	}
}

uint8_t flipbyte(uint8_t b){
    // https://stackoverflow.com/a/2602885
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

bool ppu_clock()
{
	if (ppu.scanline >= -1 && ppu.scanline < 240)
	{		
		// Background Rendering ======================================================

		if (ppu.scanline == 0 && ppu.cycle == 0)
		{
			ppu.cycle = 1;
		}

		if (ppu.scanline == -1 && ppu.cycle == 1)
		{
			ppu.registers.status.bits.verticalBlank = 0;
			ppu.registers.status.bits.spriteOverflow = 0;
			ppu.registers.status.bits.spriteZeroHit = 0;

			for (int i = 0; i < 8; i++)
			{
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}
		}


		if ((ppu.cycle >= 2 && ppu.cycle < 258) || (ppu.cycle >= 321 && ppu.cycle < 338))
		{
			UpdateShifters();
			switch ((ppu.cycle - 1) % 8)
			{
			case 0:
				LoadBackgroundShifters();
				ppu.bgNextTileId = ppuBus_read(0x2000 | (ppu.vramAddr.reg & 0x0FFF));
                

				break;
			case 2:	
				ppu.bgNextTileAttr = ppuBus_read(0x23C0 | (ppu.vramAddr.bits.nametableY << 11) 
					                                 | (ppu.vramAddr.bits.nametableX << 10) 
					                                 | ((ppu.vramAddr.bits.coarseY >> 2) << 3) 
					                                 | (ppu.vramAddr.bits.coarseX >> 2));
						
				if (ppu.vramAddr.bits.coarseY & 0x02) ppu.bgNextTileAttr >>= 4;
				if (ppu.vramAddr.bits.coarseX & 0x02) ppu.bgNextTileAttr >>= 2;
				ppu.bgNextTileAttr &= 0x03;
				break;

			case 4: 
				ppu.bgNextTileLsb = ppuBus_read((ppu.registers.ctrl.bits.patternBackground << 12) 
					                       + ((uint16_t)ppu.bgNextTileId << 4) 
					                       + (ppu.vramAddr.bits.fineY) + 0);
                
				break;
			case 6:
				ppu.bgNextTileMsb = ppuBus_read((ppu.registers.ctrl.bits.patternBackground << 12)
					                       + ((uint16_t)ppu.bgNextTileId << 4)
					                       + (ppu.vramAddr.bits.fineY) + 8);
				break;
			case 7:
				IncrementScrollX();
				break;
			}
		}
		if (ppu.cycle == 256)
		{
			IncrementScrollY();
		}

		if (ppu.cycle == 257)
		{
			LoadBackgroundShifters();
			TransferAddressX();
		}

		if (ppu.cycle == 338 || ppu.cycle == 340)
		{
			ppu.bgNextTileId = ppuBus_read(0x2000 | (ppu.vramAddr.reg & 0x0FFF));
		}

		if (ppu.scanline == -1 && ppu.cycle >= 280 && ppu.cycle < 305)
		{
			TransferAddressY();
		}


		if (ppu.cycle == 257 && ppu.scanline >= 0)
		{
			memset(spriteScanline, 0xFF, 8 * sizeof(struct sObjectAttributeEntry));

			sprite_count = 0;

			for (uint8_t i = 0; i < 8; i++)
			{
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}
			uint8_t nOAMEntry = 0;

			bSpriteZeroHitPossible = false;

			while (nOAMEntry < 64 && sprite_count < 9)
			{
				int16_t diff = ((int16_t)ppu.scanline - (int16_t)OAM[nOAMEntry].y);

				
				if (diff >= 0 && diff < (ppu.registers.ctrl.bits.spriteSize ? 16 : 8))
				{

					if (sprite_count < 8)
					{
						if (nOAMEntry == 0)
						{
							bSpriteZeroHitPossible = true;
						}

						memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry], sizeof(struct sObjectAttributeEntry));
						sprite_count++;
					}				
				}

				nOAMEntry++;
			} 

			ppu.registers.status.bits.spriteOverflow = (sprite_count > 8);

		}

		if (ppu.cycle == 340)
		{

			for (uint8_t i = 0; i < sprite_count; i++)
			{

				uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
				uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

				if (!ppu.registers.ctrl.bits.spriteSize)
				{
					if (!(spriteScanline[i].attribute & 0x80))
					{
						sprite_pattern_addr_lo = 
						  (ppu.registers.ctrl.bits.patternSprite << 12  )  
						| (spriteScanline[i].id   << 4   )  
						| (ppu.scanline - spriteScanline[i].y); 
												
					}
					else
					{
						sprite_pattern_addr_lo = 
						  (ppu.registers.ctrl.bits.patternSprite << 12  )  
						| (spriteScanline[i].id   << 4   )  
						| (7 - (ppu.scanline - spriteScanline[i].y)); 
					}

				}
				else
				{
					if (!(spriteScanline[i].attribute & 0x80))
					{
						if (ppu.scanline - spriteScanline[i].y < 8)
						{
							sprite_pattern_addr_lo = 
							  ((spriteScanline[i].id & 0x01)      << 12) 
							| ((spriteScanline[i].id & 0xFE)      << 4 )  
							| ((ppu.scanline - spriteScanline[i].y) & 0x07 ); 
						}
						else
						{
							sprite_pattern_addr_lo = 
							  ( (spriteScanline[i].id & 0x01)      << 12)  
							| (((spriteScanline[i].id & 0xFE) + 1) << 4 ) 
							| ((ppu.scanline - spriteScanline[i].y) & 0x07  ); 
						}
					}
					else
					{
						if (ppu.scanline - spriteScanline[i].y < 8)
						{
							sprite_pattern_addr_lo = 
							  ( (spriteScanline[i].id & 0x01)      << 12)    
							| (((spriteScanline[i].id & 0xFE) + 1) << 4 )    
							| (((7 - (ppu.scanline - spriteScanline[i].y)) & 0x07)); 
						}
						else
						{
							sprite_pattern_addr_lo = 
							  ((spriteScanline[i].id & 0x01)       << 12)    
							| ((spriteScanline[i].id & 0xFE)       << 4 )    
							| ((7 - (ppu.scanline - spriteScanline[i].y)) & 0x07);
						}
					}
				}
				sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

				sprite_pattern_bits_lo = ppuBus_read(sprite_pattern_addr_lo);
				sprite_pattern_bits_hi = ppuBus_read(sprite_pattern_addr_hi);
				if (spriteScanline[i].attribute & 0x40)
				{
					sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
					sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
				}
				sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
				sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
			}
		}
	}


	if (ppu.scanline >= 241 && ppu.scanline < 261)
	{
		if (ppu.scanline == 241 && ppu.cycle == 1)
		{
			ppu.registers.status.bits.verticalBlank = 1;

			if (ppu.registers.ctrl.bits.enableNmi) 
				ppu.nmi = true;
		}
	}


	// Background =============================================================
	uint8_t bgPixel = 0x00;  
	uint8_t bgPalette = 0x00; 

	if (ppu.registers.mask.bits.renderBackground)
	{
		uint16_t bit_mux = 0x8000 >> ppu.fineX;

		uint8_t p0_pixel = (ppu.bgShifterPatternLo & bit_mux) > 0;
		uint8_t p1_pixel = (ppu.bgShifterPatternHi & bit_mux) > 0;

		bgPixel = (p1_pixel << 1) | p0_pixel;

		uint8_t bg_pal0 = (ppu.bgShifterAttribLo & bit_mux) > 0;
		uint8_t bg_pal1 = (ppu.bgShifterAttribHi & bit_mux) > 0;
		bgPalette = (bg_pal1 << 1) | bg_pal0;
	}

	uint8_t fg_pixel = 0x00;  
	uint8_t fg_palette = 0x00;

	volatile uint8_t fg_priority = 0x00;
	
	if (ppu.registers.mask.bits.renderSprites)
	{
		bSpriteZeroBeingRendered = false;

		for (uint8_t i = 0; i < sprite_count; i++)
		{
			if (spriteScanline[i].x == 0) 
			{
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
				fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;
				fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;
				fg_priority = (spriteScanline[i].attribute & 0x20) == 0;
				if (fg_pixel != 0)
				{
					if (i == 0)
					{
						bSpriteZeroBeingRendered = true;
					}

					break;
				}				
			}
		}		
	}

	uint8_t pixel = 0x00;   
	uint8_t palette = 0x00; 
	if (bgPixel == 0 && fg_pixel == 0)
	{
		pixel = 0x00;
		palette = 0x00;
	}
	else if (bgPixel == 0 && fg_pixel > 0)
	{
		pixel = fg_pixel;
		palette = fg_palette;
	}
	else if (bgPixel > 0 && fg_pixel == 0)
	{
		pixel = bgPixel;
		palette = bgPalette;
	}
	else if (bgPixel > 0 && fg_pixel > 0)
	{
		if (fg_priority)
		{
			pixel = fg_pixel;
			palette = fg_palette;
		}
		else
		{
			pixel = bgPixel;
			palette = bgPalette;
		}

		if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered)
		{
			
			if (ppu.registers.mask.bits.renderBackground & ppu.registers.mask.bits.renderSprites)
			{
				if (~(ppu.registers.mask.bits.renderBackgroundLeft | ppu.registers.mask.bits.renderSpritesLeft))
				{
					if (ppu.cycle >= 9 && ppu.cycle < 258)
					{
						ppu.registers.status.bits.spriteZeroHit = 1;
					}
				}
				else
				{
					if (ppu.cycle >= 1 && ppu.cycle < 258)
					{
						ppu.registers.status.bits.spriteZeroHit = 1;
					}
				}
			}
		}
	}
		
    SpriteSetPixel(ppu.spriteScreen, ppu.cycle - 1, ppu.scanline, get_colorFromPaletteRam(palette, pixel));

	ppu.cycle++;
	if (ppu.cycle >= 341)
	{
		ppu.cycle = 0;
		ppu.scanline++;
		if (ppu.scanline >= 261)
		{
			ppu.scanline = -1;
			ppu.frameCompleted = true;
		}
	}
    return ppu.nmi;
}

