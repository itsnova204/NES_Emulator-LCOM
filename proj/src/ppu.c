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

void ppu_clock(){
  
}