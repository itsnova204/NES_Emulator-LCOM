#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include <keyboard.h>
#include <timer.h>
int get_counter();

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

  uint8_t scancode = 0x00;

  if (kbc_subscribe_int(&irq_set) != 0) return 1;

  while(scancode != BREAK_ESC) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
          continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */				
                  if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                      kbc_ih();
                      scancode = get_kbc_output();
                      kbd_print_scancode((scancode & MAKE_CODE) != 0,scancode == KBC_MULTICODE? 2: 1,&scancode); 
                  }
                  break;
              default:
                  break; /* no other notifications expected: do nothing */	
          }
      } else { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
  }

  
  return kbc_unsubscribe_int();
}

int(kbd_test_poll)() {
  uint8_t scancode = 0x00;

  while (scancode != BREAK_ESC){
    kbc_read_output(false);
    
    scancode = get_kbc_output();
    kbd_print_scancode((scancode & MAKE_CODE) != 0,scancode == KBC_MULTICODE? 2: 1,&scancode);

    tickdelay(micros_to_ticks(1000));
  }

  return kbc_restore();
}

int(kbd_test_timed_scan)(uint8_t n) {
  int time = n;
  int ipc_status, r;
  message msg;
  uint8_t kbd_irq_set;
  uint8_t timer_irq_set;

  uint8_t scancode = 0x00;

  if (timer_subscribe_int(&timer_irq_set) != 0) return 1; 
  if (kbc_subscribe_int(&kbd_irq_set) != 0) return 1;

  while(scancode != BREAK_ESC && time > 0) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
          continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */				
                  if (msg.m_notify.interrupts & kbd_irq_set) { /* subscribed interrupt */
                      kbc_ih();
                      scancode = get_kbc_output();
                      kbd_print_scancode((scancode & MAKE_CODE) != 0,scancode == KBC_MULTICODE? 2: 1,&scancode);
                      time = n;
                  }
                  if (msg.m_notify.interrupts & timer_irq_set) { /* subscribed interrupt */
                      timer_int_handler();//increases counter of how many interrupts were received;
                      if(get_counter()%60==0){
                        timer_print_elapsed_time();
                        time--;
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

  if (timer_unsubscribe_int() != 0) return -1;
  if (kbc_unsubscribe_int() != 0) return -1;
  
  return 0;
}
