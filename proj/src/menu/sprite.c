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

int (draw_hours)(int hour, int minutes, uint16_t x, uint16_t y, bool with_colon) {
    if (hour < 0 || hour > 23 || minutes < 0 || minutes > 59) {
        printf("draw_hours(): invalid hour or minutes\n");
        return 1;
    }

    int hour_tens = hour / 10;
    int hour_units = hour % 10;
    int minute_tens = minutes / 10;
    int minute_units = minutes % 10;

    if (draw_sprite(images[hour_tens].name, x, y) != 0) return 1;
    if (draw_sprite(images[hour_units].name, x + 20, y) != 0) return 1;

    if (with_colon) if (draw_sprite(COLON, x + 35, y) != 0) return 1;

    if (draw_sprite(images[minute_tens].name, x + 50, y) != 0) return 1;
    if (draw_sprite(images[minute_units].name, x + 70, y) != 0) return 1;

    return 0;
}
