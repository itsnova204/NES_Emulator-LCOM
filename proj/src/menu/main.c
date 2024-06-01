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
uint8_t scancode = 0;

#define FPS 30
#define FRAME_INTERVAL (60 / FPS)

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");


  //Sem comentario fica lento
  //lcf_trace_calls("/home/lcom/labs/g5/proj/src/trace.txt");
  //lcf_log_output("/home/lcom/labs/g5/proj/src/output.txt");

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

  int ipc_status, r;
  message msg;
  uint8_t irq_set_timer, irq_set_kbd, irq_set_mouse;

  if(timer_subscribe_int(&irq_set_timer) != 0) return 1;
  if(kbd_subscribe_int(&irq_set_kbd) != 0) return 1;
  if(mouse_write_command(ENABLE_DATA_REPORT) != 0) return 1;
  if(mouse_subscribe_int(&irq_set_mouse) != 0) return 1;

  if (timer_set_frequency(0, 60) != 0) return 1;   

  bool is_second_scancode = false;
  rtc_read_date();

  int mouse_x = 0, mouse_y = 0;

  int selected_option;	

  while(scancode != KBD_ESC_BREAK_CODE) {
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
          printf("driver_receive failed with: %d", r);
          continue;
      }

      if (is_ipc_notify(ipc_status)) {
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: 
                  if (msg.m_notify.interrupts & irq_set_kbd) {
                    kbc_ih();
                    if (!is_valid()) continue;
                    scancode = get_scancode();

                    if (is_two_byte_scancode(scancode) && !is_second_scancode) {
                      is_second_scancode = true;
                      continue;
                    } else if (is_second_scancode) {
                      is_second_scancode = false;
                    }

                    if (scancode == KBD_ESC_BREAK_CODE) break;
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

                      //DRAW DATE AND TIME
                      rtc_date_t date = rtc_get_date();
                      int day = date.day;
                      int month = date.month;
                      int year = date.year + 2000;
                      int minutes = date.minutes;
                      int hours = date.hours;

                      // blink colon every 2 seconds
                      bool draw_colon = (counter / (FRAME_INTERVAL * 32)) % 2 == 0;
                      if (draw_date(day, month, year, hours, minutes, 10, 95, draw_colon) != 0) return 1;


                      // DRAW GAME OPTIONS
                      if (draw_options(250, mouse_x, mouse_y, &selected_option) != 0) return 1;


                      // DRAW MOUSE CURSOR
                      if (draw_sprite(CURSOR, mouse_x, mouse_y) != 0) return 1;

                      swap_buffers();
                    }
                  }

                  if (msg.m_notify.interrupts & irq_set_mouse) {
                    mouse_int_handler();

                    if (mouse_sync()) {
                      struct packet pp = get_mouse_packet();

                      // update mouse position
                      mouse_x += pp.delta_x;
                      mouse_y -= pp.delta_y; // moves the cursor up

                      if (mouse_x < 0) mouse_x = 0;
                      if (mouse_y < 0) mouse_y = 0;
                      if (mouse_x > 1152) mouse_x = 1152;
                      if (mouse_y > 864) mouse_y = 864;

                      if (pp.lb && selected_option >= 0) {
                        printf("Selected option: %d\n", selected_option);
                        // handler for selected option
                        // todo
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
  if (mouse_write_command(DISABLE_DATA_REPORT) != 0) return 1;

  return 0;
}
