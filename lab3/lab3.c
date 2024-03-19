 #include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "timer.c"

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

  uint8_t scan_code = 0;
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

                    kbd_print_scancode(!(KBD_MAKE_CODE & scan_code), getScanCodeSize(scan_code), &scan_code);
                  }
                  break;
              default:
                  break; /* no other notifications expected: do nothing */	
          }
      } else { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
  }

  return kbd_unsubscribe_int();
}

int(kbd_test_poll)() {
  uint8_t scan_code = 0;

  while (scan_code != KBD_ESC_BREAK_CODE) {
    if (kbc_read_output(KBD_OUT_BUF, &scan_code)) break;
    
    kbd_print_scancode(!(KBD_MAKE_CODE & scan_code), getScanCodeSize(scan_code), &scan_code);
    
    /*if (scan_code != prev_scan_code) {
      kbd_print_scancode(!(KBD_MAKE_CODE & scan_code), getScanCodeSize(scan_code), &scan_code);
      prev_scan_code = scan_code;
    }*/
  }
  return kbd_restore();
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
