#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "KBC.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "rtc.h"
#include "mouse.h"

#include "sprite.h"

int get_counter();
uint8_t scan_code = 0;

// DEFINE FPS
#define FPS 60
#define FRAME_INTERVAL (60 / FPS)

int get_selected_option(int mouse_x, int mouse_y) {
    if (mouse_x >= 100 && mouse_x <= 300) {
        if (mouse_y >= 100 && mouse_y < 200) return 1;
        if (mouse_y >= 200 && mouse_y < 300) return 2;
        if (mouse_y >= 300 && mouse_y < 400) return 3;
    }
    return 0;
}

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
  preloadSprites(mode);

  if (set_frame_buffer(mode) != 0) return 1;
  if (set_graphic_mode(mode) != 0) return 1;

  // SETUP e ciclo de interrupcoes do timer e do teclado
  int ipc_status, r;
  message msg;
  uint8_t irq_set_timer, irq_set_kbd, irq_set_mouse;

  if(timer_subscribe_int(&irq_set_timer) != 0) return 1;
  if(kbd_subscribe_int(&irq_set_kbd) != 0) return 1;
  if(mouse_subscribe_int(&irq_set_mouse) != 0) return 1;

  if (timer_set_frequency(0, 60) != 0) return 1;   

  bool is_second_scan_code = false;
  rtc_read_date();

  while(scan_code != KBD_ESC_BREAK_CODE) {
      /* Get a request message. */
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
          printf("driver_receive failed with: %d", r);
          continue;
      }

      if (is_ipc_notify(ipc_status)) { /* received notification */
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */		
                  if (msg.m_notify.interrupts & irq_set_kbd) {
                    kbc_ih();
                    if (!is_valid()) continue;
                    scan_code = get_scancode();

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

                    if (counter % 60 == 0) {
                      rtc_read_date();
                    }

                    // DRAW NEW FRAME
                    if (counter % FRAME_INTERVAL == 0) {
                      if (draw_sprite(MENU, 0, 0) != 0) return 1;

                      // blink colon every 2 seconds
                      bool draw_colon = (counter / (FRAME_INTERVAL * 32)) % 2 == 0;
                      rtc_date_t date = rtc_get_date();
                      int minutes = date.minutes;
                      int hours = date.hours;
                      if (draw_hours(hours, minutes, 10, 95, draw_colon) != 0) return 1;

                      swap_buffers();
                    }
                  }

                  if (msg.m_notify.interrupts & irq_set_mouse) {
                    mouse_int_handler();

                    if (mouse_sync()) {
                      struct packet pp = get_mouse_packet();

                        if (pp.lb) {
                        int option = get_selected_option(pp.delta_x, pp.delta_y);
                        if (option) {
                          printf("Selected option: %d\n", option);
                          // jogo
                        }
                      }
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
  if (kbd_unsubscribe_int() != 0) return 1;
  if (mouse_unsubscribe_int() != 0) return 1;

  return 0;
}
