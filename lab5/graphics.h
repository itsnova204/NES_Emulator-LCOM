#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#ifndef _LCOM_GRAPHICS_H_
#define _LCOM_GRAPHICS_H_

int set_graphic_mode(uint16_t mode);
int set_frame_buffer(uint16_t mode);

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);



#endif
