#include "scenario.h"

#define SINGLE_PLATFORM_WIDTH 573

uint16_t speed;
int position_x, position_y;
uint16_t vg_mode;
vbe_mode_info_t vbe_mode_info;

int (init_scenario)(uint16_t mode, uint16_t speed, int x, int y) {
  position_x = x;
  position_y = y;
  vg_mode = mode;
  vbe_mode_info = get_vbe_mode_info();

  if (draw_scenario() != 0) {
    printf("init_scenario(): draw_scenario() failed \n");
    return 1;
  }

  return 0;
}

int draw_scenario() {
  if (draw_platform() != 0) {
    printf("draw_scenario(): draw_platform() failed \n");
    vg_exit();
    return 1;
  }

  return 0;
}

int draw_platform() {
  int current_position_x = position_x;
  while (current_position_x < vbe_mode_info.XResolution) {
    if (vg_draw_xpm(platform, current_position_x, position_y, vg_mode) != 0) {
      printf("draw_platform(): draw_xpm() failed \n");
      vg_exit();
      return 1;
    }
    if ((current_position_x + SINGLE_PLATFORM_WIDTH) < vbe_mode_info.XResolution) {
      current_position_x += SINGLE_PLATFORM_WIDTH;
    } else {
      current_position_x = 0;
      break;
    }
  }

  return 0;
}

int next_frame() {
  position_x -= speed;
  return 0;
}
