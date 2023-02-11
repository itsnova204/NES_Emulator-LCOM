#pragma once

/**
 * @brief Main loop of the final project
 *
 * This function receives the command line arguments as provided, except program name.
 *
 * @param argc Number of command line arguments
 * @param argv Array of "C-strings" representing each argument
 * @return Return 0 upon success and non-zero otherwise
 */
int(proj_main_loop)(int argc, char *argv[]);

/**
 * @brief Demo for printing the logo of Minix 3 in a black and white background.
 * 
 * @param mode The video mode that should be used (must be a direct color one)
 * @þaram minix3 A boolean selecting either Minix 3 (true) or Ubuntu (false) logo
 * @param grayscale Sets whether the image should be grayscale or not (XPM_GRAY_* type)
 * @param delay The number of seconds the logo should be presented
 * @return Return 0 upon success and non-zero otherwise
 */
int(proj_demo)(uint16_t mode, bool minix3, bool grayscale, uint8_t delay);
