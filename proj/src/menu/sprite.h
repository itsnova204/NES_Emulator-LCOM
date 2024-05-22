#include <lcom/lcf.h>
#include <stdint.h>

#include <stdio.h>
#include "VBE.h"
#include "graphics.h"
#include "xpm_image.h"

#ifndef _LCOM_SPRITE_H_
#define _LCOM_SPRITE_H_

#define NUM_IMAGES 12
#define XPM_TYPE_MENU XPM_8_8_8_8

/**
 * @brief Enumerates the different images that can be drawn
*/
typedef enum {
    MENU, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, COLON
} ImageName;

/**
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
*/
XpmData* (get_xpm_data)(ImageName name);

/**
 * @brief Draws a sprite on the screen
*/
int (draw_sprite)(ImageName name, uint16_t x, uint16_t y);

/**
 * @brief Draws a sprite on the screen from the bottom left corner
*/
int (draw_sprint_from_bottom_left)(ImageName name, uint16_t x, uint16_t y);

int (draw_hours)(int hour, int minutes, uint16_t x, uint16_t y, bool with_colon);

#endif
