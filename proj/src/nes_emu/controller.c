#include "controller.h"

uint8_t* ctrler_ptr;

void controller_init(uint8_t controller[2]){
  ctrler_ptr = controller;
}

uint8_t* get_ctrler_ptr(){
  return ctrler_ptr;
}

void controller_press(uint8_t controller, enum ctrler_buttons_t button){
  printf("BIT(button)%02x\n", BIT(button));
  ctrler_ptr[controller] |= BIT(button);
}

void controller_unpress(uint8_t controller, enum ctrler_buttons_t button){
  ctrler_ptr[controller] &= ~BIT(button);
}

