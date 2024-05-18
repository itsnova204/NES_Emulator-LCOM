#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "KBC.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"

#include "sprite.h"

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
  lcf_trace_calls("/home/lcom/labs/g5/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g5/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)() {
  uint16_t mode = VBE_MODE_DC_32;
  preloadSprites();

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
                      if (draw_sprite(MENU, 0, 0, mode) != 0) return 1;
                      
                      swap_buffers();
                    }
                    //
                  }
                  break;
              default:
                  break;
          }
      } else {}
  }

  if (vg_exit() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  if (kbd_unsubscribe_int() != 0) return 1;

  return 0;
}
