#include "colors.h"

uint32_t (Red)(unsigned j, uint8_t step, uint32_t first) {
  return (R(first) + j * step) % (1 << get_vbe_mode_info().RedMaskSize);
}

uint32_t (Green)(unsigned i, uint8_t step, uint32_t first) {
  return (G(first) + i * step) % (1 << get_vbe_mode_info().GreenMaskSize);
}

uint32_t (Blue)(unsigned j, unsigned i, uint8_t step, uint32_t first) {
  return (B(first) + (i + j) * step) % (1 << get_vbe_mode_info().BlueMaskSize);
}

uint32_t (R)(uint32_t first){
  return ((1 << get_vbe_mode_info().RedMaskSize) - 1) & (first >> get_vbe_mode_info().RedFieldPosition);
}

uint32_t (G)(uint32_t first){
  return ((1 << get_vbe_mode_info().GreenMaskSize) - 1) & (first >>get_vbe_mode_info().GreenFieldPosition);
}

uint32_t (B)(uint32_t first){
  return ((1 << get_vbe_mode_info().BlueMaskSize) - 1) & (first >> get_vbe_mode_info().BlueFieldPosition);
}