#include "lcf_mock.h"

enum ctrler_buttons_t{
  button_right,
  button_left,
  button_down,
  button_up,
  
  button_start,
  button_select,

  button_a,
  button_b 
};

uint8_t* get_ctrler_ptr();

void controller_init(uint8_t controller[2]);
void controller_press(uint8_t controller, enum ctrler_buttons_t button);
void controller_unpress(uint8_t controller, enum ctrler_buttons_t button);
