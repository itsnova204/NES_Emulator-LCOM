#ifndef _SCENARIO_H_
#define _SCENARIO_H_

#include <lcom/lcf.h>
#include "graphics.h"
#include "COLORS.h"

#include "platform.h"

int (init_scenario)(uint16_t mode, int speed, int y);
int draw_scenario();
int draw_platform();
int draw_next_platform_frame();

#endif
