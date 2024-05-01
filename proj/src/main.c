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
#include "image.h"

int get_counter();
uint8_t scan_code = 0;

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


  uint8_t x = 30, y = 30;
  uint8_t speed = 10;

  //if (vg_draw_rectangle(0, 0, get_vbe_mode_info().XResolution, get_vbe_mode_info().YResolution, background_color) != 0) return 1; // desenhar o fundo
  //if (vg_draw_rectangle(x, y, 30, 30, square_color) != 0) return 1; // desenhar imagem inicial
  

  int indexCat = 0;
  if (vg_draw_xpm(cat_01, x, y, mode) != 0) return 1;

  //if (vg_draw_xpm(cat_02, x + 100, y, mode) != 0) return 1;

  //if (vg_draw_xpm(cat_03, x + 200, y, mode) != 0) return 1;

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

                    //if (vg_draw_rectangle(x, y, 30, 30, background_color) != 0) return 1;  // apagar a imagem anterior
                    
                    
                    if (vg_draw_rectangle(x, y, 30, 80, 0x000000) != 0) return 1;
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
/*
                    // definir limites
                    if (x > get_vbe_mode_info().XResolution - square_size) x = get_vbe_mode_info().XResolution - square_size;
                    if (y > get_vbe_mode_info().YResolution - square_size) y = get_vbe_mode_info().YResolution - square_size;
                    
                    //if (vg_draw_rectangle(x, y, square_size, square_size, square_color) != 0) return 1;  // nova imagem*/
                  }

                  if (msg.m_notify.interrupts & irq_set_timer) {
                    timer_int_handler();
                    int counter = get_counter();
                    if (counter % 10 == 0) {    // 1/6 segundos
                      if (vg_draw_rectangle(x, y, 30, 80, 0x000000) != 0) return 1;
                      indexCat = (indexCat + 1) % 3;
                      switch (indexCat) {
                        case 0:
                          if (vg_draw_xpm(cat_01, x, y, mode) != 0) return 1;
                          break;
                        case 1:
                          if (vg_draw_xpm(cat_02, x, y, mode) != 0) return 1;
                          break;
                        case 2:
                          if (vg_draw_xpm(cat_03, x, y, mode) != 0) return 1;
                          break;
                      }
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

  if (vg_exit() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  if (kbd_unsubscribe_int() != 0) return 1;

  return 0;
}
