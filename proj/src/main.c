#include <lcom/lcf.h>
#include "./nes_emu/bus.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)(int argc, char *argv[]){
  char* cart_filePath = "/home/lcom/labs/proj/roms/nestest.nes";
   
  if (access(cart_filePath, F_OK) == 0) {
    printf("Rom found!\n");
  } else {
    printf("Rom not found!\n");
    return 1;
  }

  printf("Starting NES emulator\n");
  if(bus_init(cart_filePath)) return 1;

  while (1){
    bus_clock();
  }
  
  return bus_exit();
}

