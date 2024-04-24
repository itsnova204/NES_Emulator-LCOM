#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "graphics.h"

#ifndef _LCOM_COLORS_H_
#define _LCOM_COLORS_H_

vbe_mode_info_t get_vbe_mode_info();

uint32_t (Red)(unsigned j, uint8_t step, uint32_t first);
uint32_t (Green)(unsigned i, uint8_t step, uint32_t first);
uint32_t (Blue)(unsigned j, unsigned i, uint8_t step, uint32_t first);

uint32_t (R)(uint32_t first);
uint32_t (G)(uint32_t first);
uint32_t (B)(uint32_t first);

#endif

