/**
 * @file sprite.h
 * @brief This file contains the implementation of the sprite drawing and the xpm preloading function
 * 
*/

#include <lcom/lcf.h>
#include <stdint.h>

#include <stdio.h>
#include "VBE.h"
#include "graphics.h"
#include "xpm_image.h"

#ifndef _LCOM_SPRITE_H_
#define _LCOM_SPRITE_H_

#define NUM_IMAGES 16
#define XPM_TYPE_MENU XPM_8_8_8_8
#define OPTION_WIDTH 300
#define OPTION_HEIGHT 419

/**
 * @brief Enumerates the different images that can be drawn
*/
typedef enum {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, MENU, COLON, SLASH, CURSOR, CURSOR_SELECTED, SUPER_MARIO
} ImageName;

/**
 * @struct XpmData
 * @brief Struct that contains the xpm image and its map (color)
*/
typedef struct {
    ImageName name;
    xpm_image_t image;
    uint8_t *map;
} XpmData;

/**
 * @brief Preloads all the sprites (xpm images)
*/
void (preloadSprites)(uint16_t vg_mode);

/**
 * @brief Returns the xpm data of a given image name
 * @param name Name of the image
 * @return Pointer to the xpm data of the image, NULL if the image is not found
*/
XpmData* (get_xpm_data)(ImageName name);

/**
 * @brief Draws a sprite on the screen
 * @param name Name of the image to draw
 * @param x X coordinate of the image
 * @param y Y coordinate of the image
 * @return 0 if successful, non 0 otherwise
*/
int (draw_sprite)(ImageName name, uint16_t x, uint16_t y);

/**
 * @brief Draws a sprite on the screen from the bottom left corner
 * @param name Name of the image to draw
 * @param x X coordinate of the image
 * @param y Y coordinate of the image
 * @return 0 if successful, non 0 otherwise
*/
int (draw_sprint_from_bottom_left)(ImageName name, uint16_t x, uint16_t y);

/**
 * @brief Draws the date and time on the screen at the given coordinates with the format "dd/mm/yyyy hh:mm"
 * @param day Day of the month
 * @param month Month of the year
 * @param year Year
 * @param hour Hour of the day
 * @param minutes Minutes of the hour
 * @param x X coordinate of the date
 * @param y Y coordinate of the date
 * @param with_colon If true, the colon between the hours and minutes will be drawn. This parameter is to be able to have the blinking effect of the colon
 * @return 0 if successful, non 0 otherwise
*/
int (draw_date)(int day, int month, int year, int hour, int minutes, uint16_t x, uint16_t y, bool with_colon);

/**
 * @brief Draws the game options thumbnails on the screen
 * @param y Y coordinate of the options
 * @param x_mouse X coordinate of the mouse
 * @param y_mouse Y coordinate of the mouse
 * @param selected_option Pointer to the variable that will store the selected option, value will be -1 if no option is selected
 * @return 0 if successful, non 0 otherwise
*/
int (draw_options)(uint16_t y, uint16_t x_mouse, uint16_t y_mouse, int* selected_option);


#endif
