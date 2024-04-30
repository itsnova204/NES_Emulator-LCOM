#include "controler.h"

uint8_t* ctrler_ptr;

void controler_init(uint8_t* controller[2]){
  ctrler_ptr = controller;
}

void controler_press(uint8_t controller, enum ctrler_buttons_t button){
  ctrler_ptr[controller] |= button;
}

void controler_unpress(uint8_t controller, enum ctrler_buttons_t button){
  ctrler_ptr[controller] &= !button;
}

