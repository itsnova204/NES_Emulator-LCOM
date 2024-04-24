#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

int get_counter();

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

int (page_main_loop_x)() {
    if (timer_set_frequency(0, 60) != 0) return 1; 
    if (timer_init(1) != 0) return 1;

    return 0;
}

int main() {
    if (page_main_loop_x() != 0) return 1;

    printf("Timer init successful\n");

    return 0;
}
