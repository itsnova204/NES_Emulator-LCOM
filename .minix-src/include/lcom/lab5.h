#pragma once

#include "pixmap.h"
#include "xpm.h"
#include "vbe.h"
#include "video_gr.h"

/** @defgroup lab5 lab5
 * @{
 *
 * Functions for outputting data to screen in graphics mode
 */

/**
 * @brief Initializes the video card in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode. After delay seconds, it should go 
 *  back to Minix's default text mode.
 * 
 * @param mode 16-bit VBE mode to set
 * @param delay delay in seconds after which returns to text mode
 * @return zero on success, non-zero otherwise.
 */
int (video_test_init)(uint16_t mode, uint8_t delay);

/**
 * @brief Draws a rectangle with a given color at the specified location
 * 
 * Draws a (filled) rectangle with the specified size and color, 
 *  with its top left corner at the specified coordinates. Use the video mode
 *  specified.
 * 
 * Note that the x coordinates start at 0 and increase from left to right
 * Likewise the y coordinates start at 0 and increase from top to bottom
 * 
 * Returns upon release of the ESC key.
 * 
 * @param mode VBE graphics mode to use
 * @param x horizontal coordinate of the rectangle's upper-left vertex, starts at 0, the leftmost pixel
 * @param y vertical coordinate of the rectangle's upper-left vertex, starts at 0, the topmost pixel
 * @param width length in pixels of the rectangle along the horizontal direction
 * @param height length in pixels of the rectangle along the vertical direction
 * @param color color to set the pixel
 * @return 0 on success, non-zero otherwise
 */
int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                           uint32_t color);

/**
 * @brief Draws a rectangle-based colored pattern
 * 
 * Fills the screen with rectangles with different colors, in the specified
 *  VBE graphics mode.
 * 
 * The number of rectangles along each direction (i.e. both horizontal and 
 *  vertical) is specified in the <em>no_rectangles</em> argument. If, e.g. its 
 *  value is 3, then the pattern will be a matrix of 3 by 3 rectangles. 
 * 
 * All rectangles must have the same size. If the horizontal (vertical) 
 *  resolution is not a multiple of the <em>no_rectangles</em> argument, 
 *  then there will be a black stripe on the right (bottom) of the screen.
 *  The width of this black stripe must be minimum.
 * 
 * The color of each rectangle depends on its coordinates in the rectangle
 *  matrix (row, col).
 * 
 * If the color model is indexed then the (index of the) color of the rectangle 
 *  with coordinates (row, col) is given by:
 * <div align="left" style="background-color: #E5E5E5; white-space: pre; font-family: monospace;" class="code">	index(row, col) = (first + (row * no_rectangles + col) * step) % (1 << BitsPerPixel)
 </div>
 * 
 * If the color model is direct then the components of the rectangle with 
 *  coordinates (row, col) are given by:
 * <div align="left" style="background-color: #E5E5E5; white-space: pre; font-family: monospace;" class="code">    R(row, col) = (R(first) + col * step) % (1 << RedScreeMask) <br>	G(row, col) = (G(first) + row * step) % (1 << GreenScreeMask) <br> B(row, col) = (B(first) + (col + row) * step) % (1 << BlueScreeMask)	</div>
 * 
 * Where *MaskSize are the values of the members with that name of the VBE
 *  ModeInfoBlock struct.
 * 
 * Returns upon release of the ESC key
 * 
 * @param mode VBE graphics mode to use 
 * @param no_rectangles Number of rectangles in each of the horizontal 
 * 							and vertical direction
 * @param first Color to be used in the first rectangle (the rectangle at the
 * 				 top-left corner of the screen). 
 * @param step	 Difference between the values of the R/G component in adjacent
 * 				 	rectangles in the same row/column. For the B
 * 					component check the detailed description.
 * @return 0 on success, non-zero otherwise
 */
int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);

/**
 * @brief Draws an XPM on the screen at specified coordinates
 * 
 * Draws an XPM on the screen at specified coordinates
 *  by writing to VRAM in video mode 0x105
 * You can call the <em>read_xpm()</em> function to parse the input XPM.
 *
 * @param XPM map (assuming indexed color mode)   
 * @param x horizontal coordinate of upper-left corner, starts at 0 (leftmost pixel)  
 * @param y vertical coordinate of upper-left corner, starts at 0 (top pixel)  
 * @return 0 upon success, non-zero upon failure
 */
int (video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

/**
 * @brief Moves a pixmap on the screen along horizontal/vertical axes
 * 
 * Moves a pixmap on the screen along horizontal/vertical axes, at the specified speed and frame-rate, in video mode 0x105
 * 
 * If <em>speed</em> is positive, then it specifies the displacement in pixels between consecutive frames
 * If <em>speed</em> is negative, then it specifies the number of frames required for a 1 pixel displacement
 * 
 * @param XPM map (assuming indexed color mode) 
 * @param xi initial horizontal coordinate of upper-left corner, starts at 0 (leftmost pixel)  
 * @param yi initial vertical coordinate of upper-left corner, starts at 0 (top pixel)
 * @param xf final horizontal coordinate of upper-left corner, starts at 0 (leftmost pixel)  
 * @param yf final vertical coordinate of upper-left corner, starts at 0 (top pixel) * 
 * @param speed  speed of movement.
 * @param fr_rate  frame-rate in frames per second
 * @return 0 upon success, non-zero upon failure
 */
int (video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                      int16_t speed, uint8_t fr_rate);

/**
 * @brief Retrieves, parses and displays VBE controller information (VBE function 0x0)
 * 
 * Retrieves the VBE controller information (VBE function 0x0), parses it
 *  and calls <em>vg_display_vbe_contr_info()</em>, already provided with
 *  the LCF, to display that information on the console.
 * 
 * @return 0 upon success, non-zero upon failure
 */
int (video_test_controller)();

/** @} end of lab5 */
