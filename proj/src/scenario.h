#ifndef _SCENARIO_H_
#define _SCENARIO_H_

#include <lcom/lcf.h>
#include "graphics.h"
#include "COLORS.h"

#include "platform.h"

int (init_scenario)(uint16_t mode, uint16_t speed, int x, int y);
int draw_scenario();
int draw_platform();
int next_frame();

#endif
