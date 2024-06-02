#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "drivers/KBC.h"
#include "drivers/i8042.h"
#include "drivers/i8254.h"
#include "drivers/keyboard.h"
#include "drivers/rtc.h"
#include "drivers/mouse.h"
#include "drivers/sprite.h"
#include "drivers/uart.h"

#include "nes_emu/bus.h"
#include "nes_emu/controller.h"

void parse_controller(uint8_t byte);
int get_counter();
uint8_t scancode = 0;

#define KEYBOARD_CTRLER 0
#define SERIAL_CTRLER   0
#define PORT 1

#define FPS 30
#define FRAME_INTERVAL (60 / FPS)
#define FRAME_INTERVAL_EMULATOR (60 / 60)

#define MAKE_UP   0x48
#define BREAK_UP  0xC8
#define MAKE_LEFT 0x4B
#define BREAK_LEFT 0xCB
#define MAKE_RIGHT 0x4D
#define BREAK_RIGHT 0xCD
#define MAKE_DOWN 0x50
#define BREAK_DOWN 0xD0

#define MAKE_A 0x1E
#define BREAK_A 0x9E
#define MAKE_S 0x1F
#define BREAK_S 0x9F

#define MAKE_Z 0x2C
#define BREAK_Z 0xAC
#define MAKE_X 0x2D
#define BREAK_X 0xAD

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");


  //Sem comentario fica lento
  //lcf_trace_calls("/home/lcom/labs/proj/trace.txt");
  //lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
#define RATE BAUD_115200

uint8_t irq_set_timer, irq_set_kbd, irq_set_mouse, irq_set_uart;
bool is_second_scancode = false;

bool uart_enabled = true;

int (proj_main_loop)() {
  uint16_t mode = VBE_MODE_DC_32;
  preloadSprites(mode);
  uint8_t ctrl;

  if (set_frame_buffer(mode) != 0) return 1;
  if (set_graphic_mode(mode) != 0) return 1;

  if(uart_enabled){
    uart_init(PORT, RATE, 8, 1, ODD_PARITY);

    uart_get_int_id(PORT,&ctrl);
    if ((ctrl & IIR_RECV_DATA_AVAIL)) uart_ih(PORT);
    if(uart_set_IER(PORT, ENABLE_RECEIVER_LINE_STATUS_INTERRUPT | ENABLE_RECEIVE_DATA_INTERRUPT)) return 1;
    int bit_no;
    if(uart_subscribe_int(PORT, &bit_no)) return 1;
    irq_set_uart = BIT(bit_no);
  }

  int ipc_status, r;
  message msg;
  
  vbe_mode_info_t vbe_info = get_vbe_mode_info();
/*
  
  for (size_t i = 0; i < 10; i++){
    if(vbe_info.XResolution > scale * 256 && vbe_info.YResolution > scale * 240){
      scale++;
    }else{
      break;
    }
  }
*/
  printf("XResolution: %d\n", vbe_info.XResolution);
  uint8_t scale = 3;
  int x_offset = vbe_info.XResolution / 2 - 128*scale;
  int y_offset = vbe_info.YResolution / 2 - 120*scale;

  
  if(timer_subscribe_int(&irq_set_timer) != 0) return 1;
  if(kbd_subscribe_int(&irq_set_kbd) != 0) return 1;
  if(mouse_write_command(ENABLE_DATA_REPORT) != 0) return 1;
  if(mouse_subscribe_int(&irq_set_mouse) != 0) return 1;
  
  


  if (timer_set_frequency(0, 60) != 0) return 1;   

  rtc_read_date();

  int mouse_x = 0, mouse_y = 0;
  uint8_t byte;

  int selected_option;	

  bool emulator_running = false;
  Sprite* nes_screen = SpriteCreate(256, 240);

  while(scancode != KBD_ESC_BREAK_CODE) {
      if(emulator_running){
        while (ppu_isFrameComplete() == false) bus_clock();
      }

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
                    
                    if(emulator_running){
                        //printf("scancode: 0x%02X\n", scancode);

                        if (MAKE_Z == scancode)controller_press(KEYBOARD_CTRLER, button_a);
                        if (BREAK_Z == scancode)controller_unpress(0, button_a);
                        if (MAKE_X == scancode)controller_press(KEYBOARD_CTRLER, button_b);
                        if (BREAK_X == scancode)controller_unpress(KEYBOARD_CTRLER, button_b);
                        if (MAKE_A == scancode)controller_press(KEYBOARD_CTRLER, button_select);
                        if (BREAK_A == scancode)controller_unpress(KEYBOARD_CTRLER, button_select);
                        if (MAKE_S == scancode)controller_press(KEYBOARD_CTRLER, button_start);
                        if (BREAK_S == scancode)controller_unpress(KEYBOARD_CTRLER, button_start);

                        if (MAKE_UP == scancode)controller_press(KEYBOARD_CTRLER, button_up);
                        if (BREAK_UP == scancode)controller_unpress(KEYBOARD_CTRLER, button_up);
                        if (MAKE_LEFT == scancode)controller_press(KEYBOARD_CTRLER, button_left);
                        if (BREAK_LEFT == scancode)controller_unpress(KEYBOARD_CTRLER, button_left);
                        if (MAKE_RIGHT == scancode)controller_press(KEYBOARD_CTRLER, button_right);
                        if (BREAK_RIGHT == scancode)controller_unpress(KEYBOARD_CTRLER, button_right);
                        if (MAKE_DOWN == scancode)controller_press(KEYBOARD_CTRLER, button_down);
                        if (BREAK_DOWN == scancode)controller_unpress(KEYBOARD_CTRLER, button_down);

                        if(scancode == KBD_ESC_BREAK_CODE){
                          emulator_running = false;
                          bus_exit();
                          scancode = 0;
                          break;
                        }
                    }else{
                      if(scancode == KBD_ESC_BREAK_CODE) break;
                    }
                    
                      

                  }

                  if (msg.m_notify.interrupts & irq_set_timer) {
                    timer_int_handler();
                    int counter = get_counter();

                    if(!emulator_running){

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
                    }else{
                        if(uart_enabled){
                          while (uart_recv_front(PORT,&byte)){
                              printf("byte yes: %d\n", byte);
                              parse_controller(byte);
                          }
                        }


                      if (counter % FRAME_INTERVAL_EMULATOR == 0) {
                        nes_screen = ppu_get_screen_ptr();
                        vg_draw_color_sprite(nes_screen, x_offset, y_offset,scale);
                        swap_buffers();
                        ppu_setFrameCompleted(false);
                      }
                    }
                  }

                  if(uart_enabled){
                    if (msg.m_notify.interrupts & irq_set_uart){
                      uart_ih(PORT);
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
                      if (mouse_x > vbe_info.XResolution) mouse_x = vbe_info.XResolution;
                      if (mouse_y > vbe_info.YResolution) mouse_y = vbe_info.YResolution;

                      if (pp.lb && selected_option >= 0) {
                        if(selected_option == 0){
                          if(!emulator_running){
                            char* cart_filePath = "/home/lcom/labs/proj/roms/supermariobros.nes";
                            if (access(cart_filePath, F_OK) == 0) {
                            printf("Rom found!\n");
                            } else {
                              printf("Rom not found!\n");
                              break;
                            }

                            printf("Starting NES emulator\n");
                            if(bus_init(cart_filePath)) return 1;
                            emulator_running = true;
                          }
                        }
                        if(selected_option == 1){
                            if(!emulator_running){
                            char* cart_filePath = "/home/lcom/labs/proj/roms/soccer.nes";
                            if (access(cart_filePath, F_OK) == 0) {
                            printf("Rom found!\n");
                            } else {
                              printf("Rom not found!\n");
                              break;
                            }

                            printf("Starting NES emulator\n");
                            if(bus_init(cart_filePath)) return 1;
                            emulator_running = true;
                          }
                        }
                        if(selected_option == 2){
                            if(!emulator_running){
                            char* cart_filePath = "/home/lcom/labs/proj/roms/DonkeyKong.nes";
                            if (access(cart_filePath, F_OK) == 0) {
                            printf("Rom found!\n");
                            } else {
                              printf("Rom not found!\n");
                              break;
                            }

                            printf("Starting NES emulator\n");
                            if(bus_init(cart_filePath)) return 1;
                            emulator_running = true;
                          }
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


  if(uart_enabled){
    uart_set_IER(PORT,0);
  }

  if (bus_exit() != 0){
    printf("bus_exit() failed\n");
  };
  if (vg_exit() != 0) return 1;
  if (kbd_unsubscribe_int() != 0) return 1;
  if (mouse_unsubscribe_int() != 0) return 1;
  if (mouse_write_command(DISABLE_DATA_REPORT) != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  if(uart_unsubscribe_int(PORT)) return 1;

  return 0;
}

void parse_controller(uint8_t byte){
  controller_set(SERIAL_CTRLER, byte);
  /*if (byte & BIT(0)) controller_press(SERIAL_CTRLER, button_a);
  if (byte & BIT(1)) controller_press(SERIAL_CTRLER, button_b);
  if (byte & BIT(2)) controller_press(SERIAL_CTRLER, button_select);
  if (byte & BIT(3)) controller_press(SERIAL_CTRLER, button_start);
  if (byte & BIT(4)) controller_press(SERIAL_CTRLER, button_up);
  if (byte & BIT(5)) controller_press(SERIAL_CTRLER, button_down);
  if (byte & BIT(6)) controller_press(SERIAL_CTRLER, button_left);
  if (byte & BIT(7)) controller_press(SERIAL_CTRLER, button_right);

  if (!(byte & BIT(0))) controller_unpress(SERIAL_CTRLER, button_a);
  if (!(byte & BIT(1))) controller_unpress(SERIAL_CTRLER, button_b);
  if (!(byte & BIT(2))) controller_unpress(SERIAL_CTRLER, button_select);
  if (!(byte & BIT(3))) controller_unpress(SERIAL_CTRLER, button_start);
  if (!(byte & BIT(4))) controller_unpress(SERIAL_CTRLER, button_up);
  if (!(byte & BIT(5))) controller_unpress(SERIAL_CTRLER, button_down);
  if (!(byte & BIT(6))) controller_unpress(SERIAL_CTRLER, button_left);
  if (!(byte & BIT(7))) controller_unpress(SERIAL_CTRLER, button_right);
  */
}
