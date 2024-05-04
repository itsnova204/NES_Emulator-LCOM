#include "scenario.h"

#define SINGLE_PLATFORM_WIDTH 573

int speed_;
int position_x, position_y;
uint16_t vg_mode;
vbe_mode_info_t vbe_mode_info;

int (init_scenario)(uint16_t mode, int speed, int y) {
  if (speed <= 0) {
    printf("init_scenario(): speed must be greater than 0 \n");
    return 1;
  }

  position_x = 0;
  position_y = y;
  vg_mode = mode;
  speed_ = speed;
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
  while (true) {
    if (current_position_x < 0) {
      if (vg_draw_xpm_partial(platform, 0, position_y, (-current_position_x), vg_mode) != 0) {
        printf("draw_platform(): draw_xpm_partial() failed \n");
        vg_exit();
        return 1;
      }
    } else {
      if (vg_draw_xpm(platform, current_position_x, position_y, vg_mode) != 0) {
        printf("draw_platform(): draw_xpm() failed \n");
        vg_exit();
        return 1;
      }
    }
    
    if ((current_position_x + SINGLE_PLATFORM_WIDTH) < vbe_mode_info.XResolution) {
      current_position_x += SINGLE_PLATFORM_WIDTH;
    } else {
      break;
    }
  }

  return 0;
}

int draw_next_platform_frame() {
  position_x -= speed_;
  if (position_x < -SINGLE_PLATFORM_WIDTH) {
    position_x += SINGLE_PLATFORM_WIDTH;
  }
  printf("draw_next_platform_frame(): end position_x = %d \n", position_x);
  
  if (draw_scenario() != 0) {
    printf("draw_next_platform_frame(): draw_scenario() failed \n");
    return 1;
  }
  return 0;
}
