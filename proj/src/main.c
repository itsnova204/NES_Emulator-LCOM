#include "./nes_emu/bus.h"

int main(){
  char* cart_filePath = "../roms/nestest.nes";
  printf("Starting NES emulator\n");
  bus_init(cart_filePath);

  while (1){
    bus_clock();
  }
  
  return bus_exit();
}
