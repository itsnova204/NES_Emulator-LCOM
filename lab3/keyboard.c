int examine_scancode(uint8_t scancode) {
  if (scancode & BIT(7)) {
    return 0;
  } else {
    return 1;
  }
}