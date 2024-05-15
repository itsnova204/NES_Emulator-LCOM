#include "lcf_mock.h"
#pragma once

#define PPU_CTRL          BIT(0)
#define PPU_MASK          BIT(1)
#define PPU_STATUS        BIT(2)
#define PPU_OAM_ADDRESS   BIT(3)
#define PPU_OAM_DATA      BIT(4)
#define PPU_SCROLL        BIT(5)
#define PPU_ADDRESS       BIT(6)
#define PPU_DATA          BIT(7)

union ppu_registers{ //https://www.nesdev.org/wiki/PPU_registers
  struct{
    uint8_t PPU_CTRL_reg;
    uint8_t PPU_MASK_reg;
    uint8_t PPU_STATUS_reg;
    uint8_t OAMADDR_reg;
    uint8_t OAMDATA_reg;
    uint8_t PPU_SCROLL_reg;
    uint8_t PPU_ADDR_reg;
    uint8_t PPU_DATA_reg;   
  };

  struct{
    //PPU_CTRL_reg
    uint8_t NMI_enable : 1;
    uint8_t PPU_master_slave : 1;
    uint8_t sprite_size : 1;
    uint8_t bg_pattern_table : 1;
    uint8_t sprite_pattern_table : 1;
    uint8_t VRAM_increment : 1;
    uint8_t nametable_x : 1;
    uint8_t nametable_y : 1;

    //PPU_MASK_reg
    uint8_t greyscale : 1;
    uint8_t bg_left_column : 1;
    uint8_t sprite_left_column : 1;
    uint8_t bg_enable : 1;
    uint8_t sprite_enable : 1;
    uint8_t emphasis_red : 1;
    uint8_t emphasis_green : 1;
    uint8_t emphasis_blue : 1;

    //PPU_STATUS_reg
    uint8_t sprite_overflow : 1;
    uint8_t sprite_zero_hit : 1;
    uint8_t vertical_blank : 1;

    uint32_t : 21; //not used

    //PPU_SCROLL_reg
    uint8_t fine_x : 3;
    uint8_t : 2;
    uint8_t coarse_x : 5;
    uint8_t coarse_y : 5;
  };
};

//https://forums.nesdev.org/viewtopic.php?t=664
//loopy's registers
union loopy_register{
  struct{
    uint16_t coarse_x : 5;
    uint16_t coarse_y : 5;
    uint16_t nametable_x : 1;
    uint16_t nametable_y : 1;
    uint16_t fine_y : 3;
    uint16_t unused : 1;
  };

  uint16_t reg;
};

loopy_register vram_addr; // Active "pointer" address into nametable to extract background tile info
loopy_register temp_addr; // Temporary store of information to be "transferred" into "pointer" at various times



void ppu_clock();

uint8_t sys_readFromPPU(uint16_t addr);
void sys_writeToPPU(uint16_t addr, uint8_t data);