#include "sprite.h"

XpmData images[NUM_IMAGES];
uint16_t mode;

void (preloadSprites)(uint16_t vg_mode) {
    mode = vg_mode;

    images[0].name = ZERO;
    images[1].name = ONE;
    images[2].name = TWO;
    images[3].name = THREE;
    images[4].name = FOUR;
    images[5].name = FIVE;
    images[6].name = SIX;
    images[7].name = SEVEN;
    images[8].name = EIGHT;
    images[9].name = NINE;
    images[10].name = MENU;
    images[11].name = COLON;
    images[12].name = SLASH;
    images[13].name = CURSOR;

    images[0].map = xpm_load(zero, XPM_TYPE_MENU, &images[0].image);
    images[1].map = xpm_load(one, XPM_TYPE_MENU, &images[1].image);
    images[2].map = xpm_load(two, XPM_TYPE_MENU, &images[2].image);
    images[3].map = xpm_load(three, XPM_TYPE_MENU, &images[3].image);
    images[4].map = xpm_load(four, XPM_TYPE_MENU, &images[4].image);
    images[5].map = xpm_load(five, XPM_TYPE_MENU, &images[5].image);
    images[6].map = xpm_load(six, XPM_TYPE_MENU, &images[6].image);
    images[7].map = xpm_load(seven, XPM_TYPE_MENU, &images[7].image);
    images[8].map = xpm_load(eight, XPM_TYPE_MENU, &images[8].image);
    images[9].map = xpm_load(nine, XPM_TYPE_MENU, &images[9].image);
    images[10].map = xpm_load(menu, XPM_TYPE_MENU, &images[10].image);
    images[11].map = xpm_load(colon, XPM_TYPE_MENU, &images[11].image);
    images[12].map = xpm_load(slash, XPM_TYPE_MENU, &images[12].image);
    images[13].map = xpm_load(cursor, XPM_TYPE_MENU, &images[13].image);
}

XpmData* (get_xpm_data)(ImageName name) {
    for (int i = 0; i < NUM_IMAGES; ++i) {
        if (images[i].name == name) {
            return &images[i];
        }
    }
    return NULL;
}

int (draw_sprite)(ImageName name, uint16_t x, uint16_t y) {
    XpmData* xpm_data = get_xpm_data(name);
    if (xpm_data == NULL) {
        vg_exit();
        return 1;
    
    }
    return vg_draw_xpm(xpm_data->image, xpm_data->map, x, y, mode);
}

int (draw_sprint_from_bottom_left)(ImageName name, uint16_t x, uint16_t y) {
    XpmData* xpm_data = get_xpm_data(name);
    if (xpm_data == NULL) return 1;
    return vg_draw_xpm_from_bottom_left_corner(xpm_data->image, xpm_data->map, x, y, mode);
}

int (draw_date)(int day, int month, int year, int hour, int minutes, uint16_t x, uint16_t y, bool with_colon) {
    if (hour < 0 || hour > 23 || minutes < 0 || minutes > 59 || day < 1 || day > 31 || month < 1 || month > 12 || year < 0) {
        printf("draw_date_and_hours(): invalid date or time\n");
        return 1;
    }

    const int space = 16;


    // draw date (dd/mm/yyyy)

    int day_tens = day / 10;
    int day_units = day % 10;
    int month_tens = month / 10;
    int month_units = month % 10;
    int year_thousands = year / 1000;
    int year_hundreds = (year % 1000) / 100;
    int year_tens = (year % 100) / 10;
    int year_units = year % 10;

    if (draw_sprite(images[day_tens].name, x, y) != 0) return 1;
    if (draw_sprite(images[day_units].name, x + space, y) != 0) return 1;
    if (draw_sprite(SLASH, x + 2 * space, y) != 0) return 1;

    if (draw_sprite(images[month_tens].name, x + 3 * space, y) != 0) return 1;
    if (draw_sprite(images[month_units].name, x + 4 * space, y) != 0) return 1;
    if (draw_sprite(SLASH, x + 5 * space, y) != 0) return 1;

    if (draw_sprite(images[year_thousands].name, x + 6 * space, y) != 0) return 1;
    if (draw_sprite(images[year_hundreds].name, x + 7 * space, y) != 0) return 1;
    if (draw_sprite(images[year_tens].name, x + 8 * space, y) != 0) return 1;
    if (draw_sprite(images[year_units].name, x + 9 * space, y) != 0) return 1;

    // draw hours (hh:mm)
    int hour_tens = hour / 10;
    int hour_units = hour % 10;
    int minute_tens = minutes / 10;
    int minute_units = minutes % 10;

    if (draw_sprite(images[hour_tens].name, x + 11 * space, y) != 0) return 1;
    if (draw_sprite(images[hour_units].name, x + 12 * space, y) != 0) return 1;

    if (with_colon) if (draw_sprite(COLON, x + 13 * space, y) != 0) return 1;

    if (draw_sprite(images[minute_tens].name, x + 14 * space, y) != 0) return 1;
    if (draw_sprite(images[minute_units].name, x + 15 * space, y) != 0) return 1;

    return 0;
}
