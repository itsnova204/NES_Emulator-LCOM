// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "mouse.h"
#include "timer_utils.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (mouse_test_packet)(uint32_t cnt) {
    uint8_t mouse_irq_set;

    int ipc_status, r;
    message msg;
    if (mouse_subscribe_int(&mouse_irq_set) != 0) return 1;
    
    //if (mouse_enable_data_reporting() != 0) return 1;
    if (mouse_write_command(MOUSE_DATA_REPORTING_ENABLE_CMD) != 0) return 1;

    struct packet mouse_packet;   

    while(cnt > 0) {
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */				
                    if (msg.m_notify.interrupts & mouse_irq_set) { /* subscribed interrupt */
                        mouse_ih();
                        if (mouse_store_bytes() == 3){
                            mouse_packet = get_mouse_packet();
                            mouse_print_packet(&mouse_packet);
                            mouse_reset_packet_counter();
                            cnt--;
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


    if (mouse_write_command(MOUSE_DATA_REPORTING_DISABLE_CMD) != 0) return 1;
    if (mouse_unsubscribe_int() != 0) return 1;

    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    int elapsed_time = idle_time;

    uint8_t mouse_irq_set;
    uint8_t timer_irq_set;

    int ipc_status, r;
    message msg;

    if (mouse_subscribe_int(&mouse_irq_set) != 0) return 1;
    if (timer_subscribe_int(&timer_irq_set) != 0) return 1;
    
    //if (mouse_enable_data_reporting() != 0) return 1;
    if (mouse_write_command(MOUSE_DATA_REPORTING_ENABLE_CMD) != 0) return 1;

    struct packet mouse_packet;   

    while(elapsed_time > 0) {
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */    
                    if (msg.m_notify.interrupts & timer_irq_set) {
                        timer_int_handler();//increases counter of how many interrupts were received;
                        if(timer_get_counter()%60==0){
                            printf("Elapsed time: %d\n", elapsed_time);
                            
                            elapsed_time--;
                        }  
                    }				
                    if (msg.m_notify.interrupts & mouse_irq_set) { /* subscribed interrupt */
                        mouse_ih();
                        if (mouse_store_bytes() == 3){
                            mouse_packet = get_mouse_packet();
                            mouse_print_packet(&mouse_packet);
                            mouse_reset_packet_counter();
                            elapsed_time = idle_time;
                            timer_reset_counter(0);
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


    if (mouse_write_command(MOUSE_DATA_REPORTING_DISABLE_CMD) != 0) return 1;
    if (mouse_unsubscribe_int() != 0) return 1;
    if (timer_unsubscribe_int() != 0) return 1;

    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    uint8_t mouse_irq_set;

    int ipc_status, r;
    message msg;
    if (mouse_subscribe_int(&mouse_irq_set) != 0) return 1;
    
    //if (mouse_enable_data_reporting() != 0) return 1;
    if (mouse_write_command(MOUSE_DATA_REPORTING_ENABLE_CMD) != 0) return 1;

    




    return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 0;
}
