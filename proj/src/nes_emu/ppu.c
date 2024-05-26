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

uint8_t oam_addr = 0x00;

struct sObjectAttributeEntry OAM[64];
uint8_t* pOAM = (uint8_t*)OAM;
uint8_t* getOAM_ptr(){
    return pOAM;
}

struct sObjectAttributeEntry spriteScanline[8];

uint8_t sprite_count = 0;
uint8_t sprite_shifter_pattern_lo[8];
uint8_t sprite_shifter_pattern_hi[8];

// Sprite Zero Collision Flags
bool bSpriteZeroHitPossible = false;
bool bSpriteZeroBeingRendered = false;
//============================================

int debugcount = 0;
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

Sprite *SpriteCreate(uint16_t width, uint16_t height) {
    Sprite *sprite = (Sprite*)malloc(sizeof(sprite));
    sprite->width = width;
    sprite->height = height;
    sprite->pixels = (Color*)malloc(width*height*sizeof(Color));
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            SpriteSetPixel(sprite, i, j, ColorBuild(0,0,0));//0 0 0 black
        }
    }
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
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // Get the most significant bit of the address and offsets by the rest of the bits of the address
        data = ppu.patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF];
        
    }
    // Name table
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        enum MIRROR mirror = cart_get_mirror_type();
        
        if (mirror == VERTICAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0400 && addr <= 0x07FF) data = ppu.nameTable[1][addr & 0x03FF];
            if (addr >= 0x0800 && addr <= 0x0BFF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0C00 && addr <= 0x0FFF) data = ppu.nameTable[1][addr & 0x03FF];
        }
        else if (mirror == HORIZONTAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) data = ppu.nameTable[0][addr & 0x03FF];
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
    ppu_writeToCard(addr, data, &hijack);
    if (hijack) return;
    // Pattern table
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // This memory acts as a ROM for the PPU,
        // but for som NES ROMs, this memory is a RAM.
        ppu.patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    }
    // Name table
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        enum MIRROR mirror = cart_get_mirror_type();
        if (mirror == VERTICAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0400 && addr <= 0x07FF) ppu.nameTable[1][addr & 0x03FF] = data;
            if (addr >= 0x0800 && addr <= 0x0BFF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0C00 && addr <= 0x0FFF) ppu.nameTable[1][addr & 0x03FF] = data;
        }
        else if (mirror == HORIZONTAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) ppu.nameTable[0][addr & 0x03FF] = data;
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
			// "Odd Frame" ppu.cycle skip
			ppu.cycle = 1;
		}

		if (ppu.scanline == -1 && ppu.cycle == 1)
		{
			// Effectively start of new frame, so clear vertical blank flag
			ppu.registers.status.bits.verticalBlank = 0;

			// Clear sprite overflow flag
			ppu.registers.status.bits.spriteOverflow = 0;

			// Clear the sprite zero hit flag
			ppu.registers.status.bits.spriteZeroHit = 0;

			// Clear Shifters
			for (int i = 0; i < 8; i++)
			{
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}
		}


		if ((ppu.cycle >= 2 && ppu.cycle < 258) || (ppu.cycle >= 321 && ppu.cycle < 338))
		{
			UpdateShifters();
			
			
			// In these cycles we are collecting and working with visible data
			// The "shifters" have been preloaded by the end of the previous
			// ppu.scanline with the data for the start of this ppu.scanline. Once we
			// leave the visible region, we go dormant until the shifters are
			// preloaded for the next ppu.scanline.

			// Fortunately, for background rendering, we go through a fairly
			// repeatable sequence of events, every 2 clock cycles.
			switch ((ppu.cycle - 1) % 8)
			{
			case 0:
				// Load the current background tile pattern and attributes into the "shifter"
				LoadBackgroundShifters();

				// Fetch the next background tile ID
				// "(ppu.vramAddr.reg & 0x0FFF)" : Mask to 12 bits that are relevant
				// "| 0x2000"                 : Offset into nametable space on PPU address bus
				ppu.bgNextTileId = ppuBus_read(0x2000 | (ppu.vramAddr.reg & 0x0FFF));
                
				// Explanation:
				// The bottom 12 bits of the loopy register provide an index into
				// the 4 nametables, regardless of nametable mirroring configuration.
				// nametable_y(1) nametable_x(1) coarse_y(5) coarse_x(5)
				//
				// Consider a single nametable is a 32x32 array, and we have four of them
				//   0                1
				// 0 +----------------+----------------+
				//   |                |                |
				//   |                |                |
				//   |    (32x32)     |    (32x32)     |
				//   |                |                |
				//   |                |                |
				// 1 +----------------+----------------+
				//   |                |                |
				//   |                |                |
				//   |    (32x32)     |    (32x32)     |
				//   |                |                |
				//   |                |                |
				//   +----------------+----------------+
				//
				// This means there are 4096 potential locations in this array, which 
				// just so happens to be 2^12!
				break;
			case 2:
				// Fetch the next background tile attribute. OK, so this one is a bit
				// more involved :P

				// Recall that each nametable has two rows of cells that are not tile 
				// information, instead they represent the attribute information that
				// indicates which palettes are applied to which area on the screen.
				// Importantly (and frustratingly) there is not a 1 to 1 correspondance
				// between background tile and palette. Two rows of tile data holds
				// 64 attributes. Therfore we can assume that the attributes affect
				// 8x8 zones on the screen for that nametable. Given a working resolution
				// of 256x240, we can further assume that each zone is 32x32 pixels
				// in screen space, or 4x4 tiles. Four system palettes are allocated
				// to background rendering, so a palette can be specified using just
				// 2 bits. The attribute byte therefore can specify 4 distinct palettes.
				// Therefore we can even further assume that a single palette is
				// applied to a 2x2 tile combination of the 4x4 tile zone. The very fact
				// that background tiles "share" a palette locally is the reason why
				// in some games you see distortion in the colours at screen edges.

				// As before when choosing the tile ID, we can use the bottom 12 bits of
				// the loopy register, but we need to make the implementation "coarser"
				// because instead of a specific tile, we want the attribute byte for a 
				// group of 4x4 tiles, or in other words, we divide our 32x32 address
				// by 4 to give us an equivalent 8x8 address, and we offset this address
				// into the attribute section of the target nametable.

				// Reconstruct the 12 bit loopy address into an offset into the
				// attribute memory

				// "(ppu.vramAddr.bits.coarseX >> 2)"        : integer divide coarse x by 4, 
				//                                      from 5 bits to 3 bits
				// "((ppu.vramAddr.bits.coarseY >> 2) << 3)" : integer divide coarse y by 4, 
				//                                      from 5 bits to 3 bits,
				//                                      shift to make room for coarse x

				// Result so far: YX00 00yy yxxx

				// All attribute memory begins at 0x03C0 within a nametable, so OR with
				// result to select target nametable, and attribute byte offset. Finally
				// OR with 0x2000 to offset into nametable address space on PPU bus.				
				ppu.bgNextTileAttr = ppuBus_read(0x23C0 | (ppu.vramAddr.bits.nametableY << 11) 
					                                 | (ppu.vramAddr.bits.nametableX << 10) 
					                                 | ((ppu.vramAddr.bits.coarseY >> 2) << 3) 
					                                 | (ppu.vramAddr.bits.coarseX >> 2));
				
				// Right we've read the correct attribute byte for a specified address,
				// but the byte itself is broken down further into the 2x2 tile groups
				// in the 4x4 attribute zone.

				// The attribute byte is assembled thus: BR(76) BL(54) TR(32) TL(10)
				//
				// +----+----+			    +----+----+
				// | TL | TR |			    | ID | ID |
				// +----+----+ where TL =   +----+----+
				// | BL | BR |			    | ID | ID |
				// +----+----+			    +----+----+
				//
				// Since we know we can access a tile directly from the 12 bit address, we
				// can analyse the bottom bits of the coarse coordinates to provide us with
				// the correct offset into the 8-bit word, to yield the 2 bits we are
				// actually interested in which specifies the palette for the 2x2 group of
				// tiles. We know if "coarse y % 4" < 2 we are in the top half else bottom half.
				// Likewise if "coarse x % 4" < 2 we are in the left half else right half.
				// Ultimately we want the bottom two bits of our attribute word to be the
				// palette selected. So shift as required...				
				if (ppu.vramAddr.bits.coarseY & 0x02) ppu.bgNextTileAttr >>= 4;
				if (ppu.vramAddr.bits.coarseX & 0x02) ppu.bgNextTileAttr >>= 2;
				ppu.bgNextTileAttr &= 0x03;
				break;

				// Compared to the last two, the next two are the easy ones... :P

			case 4: 
				// Fetch the next background tile LSB bit plane from the pattern memory
				// The Tile ID has been read from the nametable. We will use this id to 
				// index into the pattern memory to find the correct sprite (assuming
				// the sprites lie on 8x8 pixel boundaries in that memory, which they do
				// even though 8x16 sprites exist, as background tiles are always 8x8).
				//
				// Since the sprites are effectively 1 bit deep, but 8 pixels wide, we 
				// can represent a whole sprite row as a single byte, so offsetting
				// into the pattern memory is easy. In total there is 8KB so we need a 
				// 13 bit address.

				// "(ppu.registers.ctrl.bits.patternBackground << 12)"  : the pattern memory selector 
				//                                         from control register, either 0K
				//                                         or 4K offset
				// "((uint16_t)ppu.bgNextTileId << 4)"    : the tile id multiplied by 16, as
				//                                         2 lots of 8 rows of 8 bit pixels
				// "(ppu.vramAddr.reg.fine_y)"                  : Offset into which row based on
				//                                         vertical scroll offset
				// "+ 0"                                 : Mental clarity for plane offset
				// Note: No PPU address bus offset required as it starts at 0x0000
				ppu.bgNextTileLsb = ppuBus_read((ppu.registers.ctrl.bits.patternBackground << 12) 
					                       + ((uint16_t)ppu.bgNextTileId << 4) 
					                       + (ppu.vramAddr.bits.fineY) + 0);
                
				break;
			case 6:
				// Fetch the next background tile MSB bit plane from the pattern memory
				// This is the same as above, but has a +8 offset to select the next bit plane
				ppu.bgNextTileMsb = ppuBus_read((ppu.registers.ctrl.bits.patternBackground << 12)
					                       + ((uint16_t)ppu.bgNextTileId << 4)
					                       + (ppu.vramAddr.bits.fineY) + 8);
				break;
			case 7:
				// Increment the background tile "pointer" to the next tile horizontally
				// in the nametable memory. Note this may cross nametable boundaries which
				// is a little complex, but essential to implement scrolling
				IncrementScrollX();
				break;
			}
		}

		// End of a visible ppu.scanline, so increment downwards...
		if (ppu.cycle == 256)
		{
			IncrementScrollY();
		}

		//...and reset the x position
		if (ppu.cycle == 257)
		{
			LoadBackgroundShifters();
			TransferAddressX();
		}

		// Superfluous reads of tile id at end of ppu.scanline
		if (ppu.cycle == 338 || ppu.cycle == 340)
		{
			ppu.bgNextTileId = ppuBus_read(0x2000 | (ppu.vramAddr.reg & 0x0FFF));
		}

		if (ppu.scanline == -1 && ppu.cycle >= 280 && ppu.cycle < 305)
		{
			// End of vertical blank period so reset the Y address ready for rendering
			TransferAddressY();
		}


		// Foreground Rendering ========================================================
		// I'm gonna cheat a bit here, which may reduce compatibility, but greatly
		// simplifies delivering an intuitive understanding of what exactly is going
		// on. The PPU loads sprite information successively during the region that
		// background tiles are not being drawn. Instead, I'm going to perform
		// all sprite evaluation in one hit. THE NES DOES NOT DO IT LIKE THIS! This makes
		// it easier to see the process of sprite evaluation.
		if (ppu.cycle == 257 && ppu.scanline >= 0)
		{
			// We've reached the end of a visible ppu.scanline. It is now time to determine
			// which sprites are visible on the next ppu.scanline, and preload this info
			// into buffers that we can work with while the ppu.scanline scans the row.

			// Firstly, clear out the sprite memory. This memory is used to store the
			// sprites to be rendered. It is not the OAM.
			memset(spriteScanline, 0xFF, 8 * sizeof(struct sObjectAttributeEntry));

			// The NES supports a maximum number of sprites per ppu.scanline. Nominally
			// this is 8 or fewer sprites. This is why in some games you see sprites
			// flicker or disappear when the scene gets busy.
			sprite_count = 0;

			// Secondly, clear out any residual information in sprite pattern shifters
			for (uint8_t i = 0; i < 8; i++)
			{
				sprite_shifter_pattern_lo[i] = 0;
				sprite_shifter_pattern_hi[i] = 0;
			}

			// Thirdly, Evaluate which sprites are visible in the next ppu.scanline. We need
			// to iterate through the OAM until we have found 8 sprites that have Y-positions
			// and heights that are within vertical range of the next ppu.scanline. Once we have
			// found 8 or exhausted the OAM we stop. Now, notice I count to 9 sprites. This
			// is so I can set the sprite overflow flag in the event of there being > 8 sprites.
			uint8_t nOAMEntry = 0;

			// New set of sprites. Sprite zero may not exist in the new set, so clear this
			// flag.
			bSpriteZeroHitPossible = false;

			while (nOAMEntry < 64 && sprite_count < 9)
			{
				// Note the conversion to signed numbers here
				int16_t diff = ((int16_t)ppu.scanline - (int16_t)OAM[nOAMEntry].y);

				// If the difference is positive then the ppu.scanline is at least at the
				// same height as the sprite, so check if it resides in the sprite vertically
				// depending on the current "sprite height mode"
				// FLAGGED
				
				if (diff >= 0 && diff < (ppu.registers.ctrl.bits.spriteSize ? 16 : 8))
				{
					// Sprite is visible, so copy the attribute entry over to our
					// ppu.scanline sprite cache. Ive added < 8 here to guard the array
					// being written to.
					if (sprite_count < 8)
					{
						// Is this sprite sprite zero?
						if (nOAMEntry == 0)
						{
							// It is, so its possible it may trigger a 
							// sprite zero hit when drawn
							bSpriteZeroHitPossible = true;
						}

						memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry], sizeof(struct sObjectAttributeEntry));
						sprite_count++;
					}				
				}

				nOAMEntry++;
			} // End of sprite evaluation for next ppu.scanline

			// Set sprite overflow flag
			ppu.registers.status.bits.spriteOverflow = (sprite_count > 8);

			// Now we have an array of the 8 visible sprites for the next ppu.scanline. By 
			// the nature of this search, they are also ranked in priority, because
			// those lower down in the OAM have the higher priority.

			// We also guarantee that "Sprite Zero" will exist in spriteScanline[0] if
			// it is evaluated to be visible. 
		}

		if (ppu.cycle == 340)
		{
			// Now we're at the very end of the ppu.scanline, I'm going to prepare the 
			// sprite shifters with the 8 or less selected sprites.

			for (uint8_t i = 0; i < sprite_count; i++)
			{
				// We need to extract the 8-bit row patterns of the sprite with the
				// correct vertical offset. The "Sprite Mode" also affects this as
				// the sprites may be 8 or 16 rows high. Additionally, the sprite
				// can be flipped both vertically and horizontally. So there's a lot
				// going on here :P

				uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
				uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

				// Determine the memory addresses that contain the byte of pattern data. We
				// only need the lo pattern address, because the hi pattern address is always
				// offset by 8 from the lo address.
				if (!ppu.registers.ctrl.bits.spriteSize)
				{
					// 8x8 Sprite Mode - The control register determines the pattern table
					if (!(spriteScanline[i].attribute & 0x80))
					{
						// Sprite is NOT flipped vertically, i.e. normal    
						sprite_pattern_addr_lo = 
						  (ppu.registers.ctrl.bits.patternSprite << 12  )  // Which Pattern Table? 0KB or 4KB offset
						| (spriteScanline[i].id   << 4   )  // Which Cell? Tile ID * 16 (16 bytes per tile)
						| (ppu.scanline - spriteScanline[i].y); // Which Row in cell? (0->7)
												
					}
					else
					{
						// Sprite is flipped vertically, i.e. upside down
						sprite_pattern_addr_lo = 
						  (ppu.registers.ctrl.bits.patternSprite << 12  )  // Which Pattern Table? 0KB or 4KB offset
						| (spriteScanline[i].id   << 4   )  // Which Cell? Tile ID * 16 (16 bytes per tile)
						| (7 - (ppu.scanline - spriteScanline[i].y)); // Which Row in cell? (7->0)
					}

				}
				else
				{
					// 8x16 Sprite Mode - The sprite attribute determines the pattern table
					if (!(spriteScanline[i].attribute & 0x80))
					{
						// Sprite is NOT flipped vertically, i.e. normal
						if (ppu.scanline - spriteScanline[i].y < 8)
						{
							// Reading Top half Tile
							sprite_pattern_addr_lo = 
							  ((spriteScanline[i].id & 0x01)      << 12)  // Which Pattern Table? 0KB or 4KB offset
							| ((spriteScanline[i].id & 0xFE)      << 4 )  // Which Cell? Tile ID * 16 (16 bytes per tile)
							| ((ppu.scanline - spriteScanline[i].y) & 0x07 ); // Which Row in cell? (0->7)
						}
						else
						{
							// Reading Bottom Half Tile
							sprite_pattern_addr_lo = 
							  ( (spriteScanline[i].id & 0x01)      << 12)  // Which Pattern Table? 0KB or 4KB offset
							| (((spriteScanline[i].id & 0xFE) + 1) << 4 )  // Which Cell? Tile ID * 16 (16 bytes per tile)
							| ((ppu.scanline - spriteScanline[i].y) & 0x07  ); // Which Row in cell? (0->7)
						}
					}
					else
					{
						// Sprite is flipped vertically, i.e. upside down
						if (ppu.scanline - spriteScanline[i].y < 8)
						{
							// Reading Top half Tile
							sprite_pattern_addr_lo = 
							  ( (spriteScanline[i].id & 0x01)      << 12)    // Which Pattern Table? 0KB or 4KB offset
							| (((spriteScanline[i].id & 0xFE) + 1) << 4 )    // Which Cell? Tile ID * 16 (16 bytes per tile)
							| (7 - (ppu.scanline - spriteScanline[i].y) & 0x07); // Which Row in cell? (0->7)
						}
						else
						{
							// Reading Bottom Half Tile
							sprite_pattern_addr_lo = 
							  ((spriteScanline[i].id & 0x01)       << 12)    // Which Pattern Table? 0KB or 4KB offset
							| ((spriteScanline[i].id & 0xFE)       << 4 )    // Which Cell? Tile ID * 16 (16 bytes per tile)
							| (7 - (ppu.scanline - spriteScanline[i].y) & 0x07); // Which Row in cell? (0->7)
						}
					}
				}

				// Phew... XD I'm absolutely certain you can use some fantastic bit 
				// manipulation to reduce all of that to a few one liners, but in this
				// form it's easy to see the processes required for the different
				// sizes and vertical orientations

				// Hi bit plane equivalent is always offset by 8 bytes from lo bit plane
				sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

				// Now we have the address of the sprite patterns, we can read them
				sprite_pattern_bits_lo = ppuBus_read(sprite_pattern_addr_lo);
				sprite_pattern_bits_hi = ppuBus_read(sprite_pattern_addr_hi);

				// If the sprite is flipped horizontally, we need to flip the 
				// pattern bytes. 
				if (spriteScanline[i].attribute & 0x40)
				{

					// Flip Patterns Horizontally
					sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
					sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
				}

				// Finally! We can load the pattern into our sprite shift registers
				// ready for rendering on the next ppu.scanline
				sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
				sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
			}
		}
	}

	if (ppu.scanline == 240)
	{
		// Post Render Scanline - Do Nothing!
	}

	if (ppu.scanline >= 241 && ppu.scanline < 261)
	{
		if (ppu.scanline == 241 && ppu.cycle == 1)
		{
			// Effectively end of frame, so set vertical blank flag
			ppu.registers.status.bits.verticalBlank = 1;

			// If the control register tells us to emit a NMI when
			// entering vertical blanking period, do it! The CPU
			// will be informed that rendering is complete so it can
			// perform operations with the PPU knowing it wont
			// produce visible artefacts
			if (ppu.registers.ctrl.bits.enableNmi) 
				ppu.nmi = true;
		}
	}


	// Composition - We now have background & foreground pixel information for this ppu.cycle

	// Background =============================================================
	uint8_t bg_pixel = 0x00;   // The 2-bit pixel to be rendered
	uint8_t bg_palette = 0x00; // The 3-bit index of the palette the pixel indexes

	// We only render backgrounds if the PPU is enabled to do so. Note if 
	// background rendering is disabled, the pixel and palette combine
	// to form 0x00. This will fall through the colour tables to yield
	// the current background colour in effect
	if (ppu.registers.mask.bits.renderBackground)
	{
		// Handle Pixel Selection by selecting the relevant bit
		// depending upon fine x scolling. This has the effect of
		// offsetting ALL background rendering by a set number
		// of pixels, permitting smooth scrolling
		uint16_t bit_mux = 0x8000 >> ppu.fineX;

		// Select Plane pixels by extracting from the shifter 
		// at the required location. 
		uint8_t p0_pixel = (ppu.bgShifterPatternLo & bit_mux) > 0;
		uint8_t p1_pixel = (ppu.bgShifterPatternHi & bit_mux) > 0;

		// Combine to form pixel index
		bg_pixel = (p1_pixel << 1) | p0_pixel;

		// Get palette
		uint8_t bg_pal0 = (ppu.bgShifterAttribLo & bit_mux) > 0;
		uint8_t bg_pal1 = (ppu.bgShifterAttribHi & bit_mux) > 0;
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}

	// Foreground =============================================================
	uint8_t fg_pixel = 0x00;   // The 2-bit pixel to be rendered
	uint8_t fg_palette = 0x00; // The 3-bit index of the palette the pixel indexes
	uint8_t fg_priority = 0x00;// A bit of the sprite attribute indicates if its
							   // more important than the background
	if (ppu.registers.mask.bits.renderSprites)
	{
		// Iterate through all sprites for this ppu.scanline. This is to maintain
		// sprite priority. As soon as we find a non transparent pixel of
		// a sprite we can abort

		bSpriteZeroBeingRendered = false;

		for (uint8_t i = 0; i < sprite_count; i++)
		{
			// Scanline ppu.cycle has "collided" with sprite, shifters taking over
			if (spriteScanline[i].x == 0) 
			{
				// Note Fine X scrolling does not apply to sprites, the game
				// should maintain their relationship with the background. So
				// we'll just use the MSB of the shifter
				
				// Determine the pixel value...
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
				fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

				// Extract the palette from the bottom two bits. Recall
				// that foreground palettes are the latter 4 in the 
				// palette memory.
				fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;
				fg_priority = (spriteScanline[i].attribute & 0x20) == 0;

				// If pixel is not transparent, we render it, and dont
				// bother checking the rest because the earlier sprites
				// in the list are higher priority
				if (fg_pixel != 0)
				{
					if (i == 0) // Is this sprite zero?
					{
						bSpriteZeroBeingRendered = true;
					}

					break;
				}				
			}
		}		
	}

	// Now we have a background pixel and a foreground pixel. They need
	// to be combined. It is possible for sprites to go behind background
	// tiles that are not "transparent", yet another neat trick of the PPU
	// that adds complexity for us poor emulator developers...

	uint8_t pixel = 0x00;   // The FINAL Pixel...
	uint8_t palette = 0x00; // The FINAL Palette...

	if (bg_pixel == 0 && fg_pixel == 0)
	{
		// The background pixel is transparent
		// The foreground pixel is transparent
		// No winner, draw "background" colour
		pixel = 0x00;
		palette = 0x00;
	}
	else if (bg_pixel == 0 && fg_pixel > 0)
	{
		// The background pixel is transparent
		// The foreground pixel is visible
		// Foreground wins!
		pixel = fg_pixel;
		palette = fg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel == 0)
	{
		// The background pixel is visible
		// The foreground pixel is transparent
		// Background wins!
		pixel = bg_pixel;
		palette = bg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel > 0)
	{
		// The background pixel is visible
		// The foreground pixel is visible
		// Hmmm...
		if (fg_priority)
		{
			// Foreground cheats its way to victory!
			pixel = fg_pixel;
			palette = fg_palette;
		}
		else
		{
			// Background is considered more important!
			pixel = bg_pixel;
			palette = bg_palette;
		}

		// Sprite Zero Hit detection
		if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered)
		{
			// Sprite zero is a collision between foreground and background
			// so they must both be enabled
			
			if (ppu.registers.mask.bits.renderBackground & ppu.registers.mask.bits.renderSprites)
			{
				// The left edge of the screen has specific switches to control
				// its appearance. This is used to smooth inconsistencies when
				// scrolling (since sprites x coord must be >= 0)
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

	// Now we have a final pixel colour, and a palette for this ppu.cycle
	// of the current ppu.scanline. Let's at long last, draw that ^&%*er :P
    SpriteSetPixel(ppu.spriteScreen, ppu.cycle - 1, ppu.scanline, get_colorFromPaletteRam(palette, pixel));

	// Advance renderer - it never stops, it's relentless
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
Sprite* ppu_get_screen_ptr() {
		return ppu.spriteScreen;
}
