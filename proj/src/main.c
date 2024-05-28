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
#include "nes_emu/controler.h"

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

#define MAKE_UP   0x4B
#define BREAK_UP  0xCB
#define MAKE_LEFT 0x4D
#define BREAK_LEFT 0xCD
#define MAKE_RIGHT 0x4D
#define BREAK_RIGHT 0xCD
#define MAKE_DOWN 0x50
#define BREAK_DOWN 0xD0

#define MAKE_A 0x1E
#define BREAK_A 0x9E
#define MAKE_S 0x1F
#define BREAK_S 0x9F

extern uint8_t controller[2];

int (proj_main_loop)() {
  Sprite* nes_screen = SpriteCreate(256, 240);

  char* cart_filePath = "/home/lcom/labs/proj/roms/Super_mario_brothers.nes";
   
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

  // SETUP e ciclo de interrupcoes do timer e do teclado
  int ipc_status, r;
  message msg;
  uint8_t irq_set_timer, irq_set_kbd;

  if(timer_subscribe_int(&irq_set_timer) != 0) return 1;
  if(kbd_subscribe_int(&irq_set_kbd) != 0) return 1;

  if (timer_set_frequency(0, 60) != 0) return 1;   

  bool is_second_scan_code = false;
  while( scan_code != KBD_ESC_BREAK_CODE ) {
        while (ppu_isFrameComplete() == false) bus_clock();
        ppu_setFrameCompleted(false);
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
          continue;
      }

      if (is_ipc_notify(ipc_status)) { /* received notification */
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */		
                  if (msg.m_notify.interrupts & irq_set_kbd) {
                    kbc_ih();
                    if (!is_valid()) continue;
                    scan_code = get_scan_code();
                    if (is_two_byte_scan_code(scan_code) && !is_second_scan_code) {
                      is_second_scan_code = true;
                      continue;
                    } else if (is_second_scan_code) {
                      is_second_scan_code = false;
                    }

                    if (scan_code == KBD_ESC_BREAK_CODE) break;
                    
                  }

                  if (msg.m_notify.interrupts & irq_set_timer) {
                    timer_int_handler();
                    int counter = get_counter();

                    // DRAW NEW FRAME
                    if (counter % FRAME_INTERVAL == 0) {
                      if (draw_sprite(MENU, 0, 0) != 0) return 1;
                      nes_screen = ppu_get_screen_ptr();
                      vg_draw_color_sprite(nes_screen, 150, 100, 2);
                      // blink colon every 2 seconds
                      bool draw_colon = (counter / (FRAME_INTERVAL * 32)) % 2 == 0;
                      if (draw_hours(12, 34, 10, 95, draw_colon) != 0) return 1;

                      swap_buffers();
                    }

                  }
                  break;
              default:
                  break;
          }
      } else {}
  }

  if (vg_exit() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  sleep(3);
  if (kbd_unsubscribe_int() != 0) return 1;
  
  if (bus_exit() != 0) return 1;
  
  return 0;
}
