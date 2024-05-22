#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "drivers/KBC.h"
#include "drivers/i8042.h"
#include "drivers/i8254.h"
#include "drivers/keyboard.h"

#include "drivers/sprite.h"

#include "nes_emu/ppu.h"
#include "nes_emu/bus.h"

int get_counter();
uint8_t scan_code = 0;

// DEFINE FPS
#define FPS 60
#define FRAME_INTERVAL (60 / FPS)

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

void populate_nes_screen(Sprite* nes_screen) {
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 240; j++) {
      SpriteSetPixel(nes_screen, i, j, ColorBuild(100, 0, 0));
    }
  }
}

int (proj_main_loop)() {
  Sprite* nes_screen = SpriteCreate(256, 240);

  char* cart_filePath = "/home/lcom/labs/proj/roms/nestest.nes";
   
  if (access(cart_filePath, F_OK) == 0) {
    printf("Rom found!\n");
  } else {
    printf("Rom not found!\n");
    return 1;
  }

  printf("Starting NES emulator\n");
  if(bus_init(cart_filePath)) return 1;
  

  uint16_t mode = VBE_MODE_DC_32;
  preloadSprites(mode);

  if (set_frame_buffer(mode) != 0) return 1;
  if (set_graphic_mode(mode) != 0) return 1;




  draw_sprite(MENU, 0, 0);
  swap_buffers();
  
  int counter = 60000;
  while (counter > 0) {
      bus_clock();
      nes_screen = ppu_get_screen_ptr();
      vg_draw_color_sprite(nes_screen, 0, 0);
      counter--;
      //printf("counter: %d\n", counter);
  }
  

  if (vg_exit() != 0) return 1;


  return 0;
}
