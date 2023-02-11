#pragma once

#include <stdint.h>

/** @addtogroup xpm
 *  @{
 */

// a line containing XPM information (both the data and the pointer are constant)
typedef char const *const xpm_row_t;

// ensure that neither the pointer nor the content of an XPM pixmap can be changed
typedef xpm_row_t *const xpm_map_t;

/**
 * @brief Transparency color macros
 * 
 * Define the color that is to be considered as transparency for a given output format
 */
#define TRANSPARENCY_COLOR_1_5_5_5 0x8000
#define TRANSPARENCY_COLOR_8_8_8_8 0xFF000000
#define CHROMA_KEY_GREEN_888 0x00b140
#define CHROMA_KEY_GREEN_565 0x0588

/**
 * @brief Output formats supported by xpm_load() function
 * 
 * XPM_INDEXED can only be used for XPM pixmaps encoded using indexed color mode.
 * INVALID_XPM is set when the XPM pixmap is not valid.
 */
enum xpm_image_type {
  XPM_INDEXED,
  XPM_1_5_5_5,
  XPM_5_6_5,
  XPM_8_8_8,
  XPM_8_8_8_8,
  XPM_GRAY_1_5_5_5,
  XPM_GRAY_5_6_5,
  XPM_GRAY_8_8_8,
  XPM_GRAY_8_8_8_8,
  INVALID_XPM
};

/**
 * @brief Struct that stores the information about an image.
 * 
 * type is the output format
 * width is the number of horizontal pixels
 * height is the number of vertical pixels
 * size is the number of bytes [width * height * bytes_per_pixel]
 * data is the pointer to the start of the image data
 */
typedef struct {
  enum xpm_image_type type;
  uint16_t width;
  uint16_t height;
  size_t size;
  uint8_t *data;
} xpm_image_t;

/**
 * Reads a xpm-like pixmap defined in "map". Returns the address of the
 * allocated memory to where the image was read in the specified format (type).
 * Updates "img" with the image information. Returns NULL on error.
 * 
 * NOTE: If the required format does not support alpha channel (transparency/opacity),
 *       a corresponding chroma key green will be used instead (except for indexed mode).
 * 
 * Usage example, assuming that "my_xpm" stores the content of the XPM file:
 * <pre>
 *   // can be XPM_INDEXED, XPM_1_5_5_5, XPM_5_6_5, XPM_8_8_8 or XPM_8_8_8_8
 *   enum xpm_image_type type = XPM_5_6_5;
 *   xpm_image_t img;
 *   uint8_t *sprite = xpm_load(my_xpm, type, &img);
 * </pre>
 */
uint8_t *xpm_load(xpm_map_t map, enum xpm_image_type type, xpm_image_t *img);

/**
 * Frees the dynamically allocated data pointed by data field.
 * Returns false on error.
 */
bool xpm_free_data(xpm_image_t *img);

/**
 * Returns the color used as transparency for a given XPM image type or 0 if the type
 * is not valid.
 */
uint32_t xpm_transparency_color(enum xpm_image_type type);

/** @} end of xpm */
