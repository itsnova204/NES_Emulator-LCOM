#include "controler.h"

uint8_t* ctrler_ptr;

void controler_init(uint8_t controller[2]){
  ctrler_ptr = controller;
}

uint8_t* get_ctrler_ptr(){
  return ctrler_ptr;
}

void controler_press(uint8_t controller, enum ctrler_buttons_t button){
  ctrler_ptr[controller] |= BIT(button);
}

void controler_unpress(uint8_t controller, enum ctrler_buttons_t button){
  ctrler_ptr[controller] &= ~BIT(button);
}

