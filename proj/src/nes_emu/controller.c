#include "controller.h"

uint8_t* ctrler_ptr;

void controller_init(uint8_t controller[2]){
  ctrler_ptr = controller;
}

void controller_set(uint8_t controller, uint8_t value){
    value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
    value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
    value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
  ctrler_ptr[controller] = value;
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

