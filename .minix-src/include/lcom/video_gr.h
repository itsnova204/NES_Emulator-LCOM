#pragma once

#include <stdint.h>

#include "vbe.h"

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode using a linear frame buffer, maps VRAM to the process' 
 *  address space and initializes static global variables with the 
 *  resolution of the screen, and the color depth (i.e the no. of bits
 *  per pixel). Initially, you should call function <em>vbe_get_mode_info()</em>
 *  provided by the LCF, to get this information. Later, you can implement
 *  your own version of this function.
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *(vg_init)(uint16_t mode);

/**
 * @brief Draws a horizontal line by changing the video RAM
 * 
 * Draws a horizontal line with the specified length and color, 
 *  starting at the specified coordinates.
 * 
 * Note that the x coordinates start at 0 and increase from left to right<br>
 * Likewise the y coordinates start at 0 and increase from top to bottom
 * 
 * @param x horizontal coordinate of the initial point
 * @param y vertical coordinate of the initial point
 * @param len line's length in pixels
 * @param color color to set the pixel
 * @param addr virtual address of the VRAM location with the initial point
 * @return 0 on success, non-zero otherwise
 */

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * @brief Draws a filled rectangle by changing the video RAM.
 *
 * Draws a filled rectangle with the specified width, height and color, 
 *  starting at the specified coordinates.
 *
 * Note that the x coordinates start at 0 and increase from left to right.<br>
 * Likewise, the y coordinates start at 0 and increase from top to bottom.
 * 
 * @param x	horizontal coordinate of the rectangle's top-left vertex
 * @param y	vertical coordinate of the rectangle's top-left vertex
 * @param width	 rectangle's width in pixels
 * @param height rectangle's height in pixels
 * @param color	color to set the pixel
 * 
 * @return 0 on success, non-zero otherwise
 */
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * This function is already provided by the LCF, you must <b>not</b> implement it.
 * 
 * @return 0 upon success, non-zero upon failure
 */
int (vg_exit)(void);

/**
 * @brief Displays some fields of the VBE controller information
 * 
 * This function is already provided by the LCF, you <b>need not</b> implement it
 * 
 * @param info_p pointer to struct with the values of the fields of the VBE controller information to display
 * 
 * @return 0 upon success, non-zero upon failure
 */
int (vg_display_vbe_contr_info)(struct vg_vbe_contr_info *info_p);

/** @} end of video_gr */
