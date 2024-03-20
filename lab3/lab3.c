 #include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "timer.c"

uint8_t scan_code = 0;
bool flag_two_byte = false;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
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

                    flag_two_byte = is_two_byte_scan_code(scan_code);
                    if (!flag_two_byte) {
                      kbd_print_scancode(!(KBD_MAKE_CODE & scan_code), getScanCodeSize(scan_code), &scan_code);
                    }
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

int(kbd_test_poll)() {
  while (scan_code != KBD_ESC_BREAK_CODE) {
    if (kbc_read_output(KBD_OUT_BUF, &scan_code, false)) break;

    flag_two_byte = is_two_byte_scan_code(scan_code);
    if (!flag_two_byte) {
      kbd_print_scancode(!(KBD_MAKE_CODE & scan_code), getScanCodeSize(scan_code), &scan_code);
    }
  }

  return kbd_restore();
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status, r, timer_counter = get_counter();
  message msg;
  uint8_t timer_irq_set, kbd_irq_set, timer_seconds = n;

  if (timer_subscribe_int(&timer_irq_set) != 0) return 1;
  if (kbd_subscribe_int(&kbd_irq_set) != 0) return 1;

  while (scan_code != KBD_ESC_BREAK_CODE && timer_seconds > 0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
      printf("keyboard driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */	
                  if (msg.m_notify.interrupts & timer_irq_set) {
                    timer_int_handler();
                    timer_counter = get_counter();
                    if (timer_counter % 60 == 0) timer_seconds--;
                  }

                  if (msg.m_notify.interrupts & kbd_irq_set) { /* subscribed interrupt */
                    kbc_ih();
                    if (!is_valid()) continue;

                    scan_code = get_scan_code();

                    flag_two_byte = is_two_byte_scan_code(scan_code);
                    if (!flag_two_byte) {
                      kbd_print_scancode(!(KBD_MAKE_CODE & scan_code), getScanCodeSize(scan_code), &scan_code);
                    }

                    timer_seconds = n;
                    reset_counter();
                    timer_counter = get_counter();
                  }
          }
      } else { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
  }
  if (kbd_unsubscribe_int()) return 1;
  if (timer_unsubscribe_int()) return 1;

  return 0;
}
