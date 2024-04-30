// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "VBE.h"
#include "graphics.h"
#include "KBC.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "KBC.h"

uint8_t scan_code = 0;
int get_counter();

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g5/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g5/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(keyboard_ESC)() {
  int ipc_status, r;
  message msg;
  uint8_t irq_set;

  if (kbd_subscribe_int(&irq_set) != 0) return 1;

  while (scan_code != KBD_ESC_BREAK_CODE) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
      printf("keyboard driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */				
                  if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                    kbc_ih();
                    if (!is_valid()) continue;

                    scan_code = get_scan_code();

                    if (print_scancode(scan_code)) return 1;
                  }
                  break;
              default:
                  break; /* no other notifications expected: do nothing */	
          }
      } else {
      }
  }

  return kbd_unsubscribe_int();
}


int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (set_graphic_mode(mode) != 0) return 1;
  sleep(delay);

  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  
  if (set_frame_buffer(mode) != 0) return 1;
  if (set_graphic_mode(mode) != 0) return 1;
  if (vg_draw_rectangle(x, y, width, height, color) != 0) return 1;

  if (keyboard_ESC() != 0) return 1;

  return vg_exit();
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (set_frame_buffer(mode) != 0) return 1;
  if (set_graphic_mode(mode) != 0) return 1;
  if (vg_draw_pattern(mode, no_rectangles, first, step) != 0) return 1;

  if (keyboard_ESC() != 0) return 1;

  return vg_exit();
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (set_frame_buffer(VBE_MODE_INDEXED) != 0) return 1; // it is required to use this mode
  if (set_graphic_mode(VBE_MODE_INDEXED) != 0) return 1;

  if (vg_draw_xpm(xpm, x, y) != 0) return 1;

  if (keyboard_ESC() != 0) return 1;

  return vg_exit();
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  // considerar apenas moviementos retos
  if (xi != xf && yi != yf) {
    printf("video_test_move(): only straight movements are allowed\n");
    return 1;
  }
  bool isMovementHorizontal = true; // true if the movement is horizontal, false if it is vertical

  if (xi == xf) isMovementHorizontal = false;
  else if (yi == yf) isMovementHorizontal = true;
  else return 1;

  // definir o buffer de video
  if (set_frame_buffer(VBE_MODE_DC_24) != 0) return 1; // it is required to use this mode
  if (set_graphic_mode(VBE_MODE_DC_24D) != 0) return 1;

  // SETUP e ciclo de interrupcoes do timer e do teclado
  int ipc_status, r;
  message msg;
  uint8_t irq_set_timer, irq_set_kbd;

  // subcricao das interrupcoes do timer e do teclado
  if(timer_subscribe_int(&irq_set_timer) != 0) return 1;
  if(kbd_subscribe_int(&irq_set_kbd) != 0) return 1;

  // definir o frame rate do timer
  if (timer_set_frequency(0, fr_rate) != 0) return 1;   

  bool finished = false;  // flag que indica se o movimento terminou
  while( scan_code != KBD_ESC_BREAK_CODE && !finished ) {
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
                  }

                  if (msg.m_notify.interrupts & irq_set_timer) {
                    //if (vg_clear_screen() != 0) return 1; // limpar o ecrã (apagar a imagem)
                    
                    // atualizar coordenadas
                    if (isMovementHorizontal) {
                        xi += speed;
                        if (xi > xf) xi = xf; // se passar do limite, coloca no limite
                    } else {
                        yi += speed;
                        if (yi > yf) yi = yf;
                    }
                    
                    // desenhar imagem
                    if (vg_draw_xpm(xpm, xi, yi) != 0) return 1;

                    // verificar se o movimento terminou
                    if (xi == xf && yi == yf) finished = true;
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

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
