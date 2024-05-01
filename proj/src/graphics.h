#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#ifndef _LCOM_GRAPHICS_H_
#define _LCOM_GRAPHICS_H_

vbe_mode_info_t (get_vbe_mode_info)();
int set_graphic_mode(uint16_t mode);
int set_frame_buffer(uint16_t mode);

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int vg_draw_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y, uint16_t mode);
int vg_clear_screen();

uint32_t (Red)(unsigned j, uint8_t step, uint32_t first);
uint32_t (Green)(unsigned i, uint8_t step, uint32_t first);
uint32_t (Blue)(unsigned j, unsigned i, uint8_t step, uint32_t first);
uint32_t (R)(uint32_t first);
uint32_t (G)(uint32_t first);
uint32_t (B)(uint32_t first);

#endif
