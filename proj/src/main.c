#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "VBE.h"
#include "graphics.h"
#include "KBC.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "KBC.h"

int get_counter();
uint8_t scan_code = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g5/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g5/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_init)(uint8_t time) {
  
  int ipc_status, r;
  message msg;
  uint8_t irq_set;

  if(timer_subscribe_int(&irq_set) != 0) return 1;
  
  while( time > 0 ) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
          continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */				
                  if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                    timer_int_handler();
                    int counter = get_counter();
                    if (counter%60 == 0) {
                      time--;
                      timer_print_elapsed_time();
                    }
                  }
                  break;
              default:
                  break; /* no other notifications expected: do nothing */	
          }
      } else { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
  }

  if (timer_unsubscribe_int() != 0) return 1;

  return 0;
}

int (proj_main_loop)() {
  // definir o buffer de video
  if (set_frame_buffer(VBE_MODE_DC_24) != 0) return 1; // it is required to use this mode
  if (set_graphic_mode(VBE_MODE_DC_24) != 0) return 1;

  // SETUP e ciclo de interrupcoes do timer e do teclado
  int ipc_status, r;
  message msg;
  uint8_t irq_set_timer, irq_set_kbd;

  // subcricao das interrupcoes do timer e do teclado
  if(timer_subscribe_int(&irq_set_timer) != 0) return 1;
  if(kbd_subscribe_int(&irq_set_kbd) != 0) return 1;

  // definir o frame rate do timer
  if (timer_set_frequency(0, 60) != 0) return 1;   

  bool is_second_scan_code = false;
  uint32_t square_color = 0x000000, background_color = 0xffffff;
  uint16_t square_size = 30, speed = 15, x = 50, y = 50;

  if (vg_draw_rectangle(0, 0, get_vbe_mode_info().XResolution, get_vbe_mode_info().YResolution, background_color) != 0) return 1; // desenhar o fundo
  if (vg_draw_rectangle(x, y, 30, 30, square_color) != 0) return 1; // desenhar imagem inicial

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

                    if (vg_draw_rectangle(x, y, 30, 30, background_color) != 0) return 1;  // apagar a imagem anterior

                    if (scan_code == KBD_ESC_BREAK_CODE) break;
                    if (scan_code == 0x50) {  //down arrow
                      y += speed;
                    } else if (scan_code == 0x48) {  //up arrow
                      if (y >= speed) y -= speed; // definir limite das bordas
                      else y = 0;
                    } else if (scan_code == 0x4b) {  //left arrow
                      if (x >= speed) x -= speed;
                      else x = 0;
                    } else if (scan_code == 0x4d) {  //right arrow
                      x += speed;
                    }

                    // definir limites
                    if (x > get_vbe_mode_info().XResolution - square_size) x = get_vbe_mode_info().XResolution - square_size;
                    if (y > get_vbe_mode_info().YResolution - square_size) y = get_vbe_mode_info().YResolution - square_size;
                    
                    if (vg_draw_rectangle(x, y, square_size, square_size, square_color) != 0) return 1;  // nova imagem
                  }
                  break;
              default:
                  break; /* no other notifications expected: do nothing */	
          }
      } else { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
  }

  if (vg_exit() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  if (kbd_unsubscribe_int() != 0) return 1;

  return 0;
}
