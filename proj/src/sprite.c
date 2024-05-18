#include "sprite.h"

XpmData images[NUM_IMAGES];

void (preloadSprites)() {
  images[0].name = MENU;
  images[0].map = xpm_load(menu, XPM_TYPE_MENU, &images[0].image);
}

XpmData* (get_xpm_data)(ImageName name) {
    for (int i = 0; i < NUM_IMAGES; ++i) {
        if (images[i].name == name) {
            return &images[i];
        }
    }
    return NULL;
}

int (draw_sprite)(ImageName name, uint16_t x, uint16_t y, uint16_t mode) {
    XpmData* xpm_data = get_xpm_data(name);
    if (xpm_data == NULL) return 1;
    return vg_draw_xpm(xpm_data->image, xpm_data->map, x, y, mode);
}

int (draw_sprint_from_bottom_left)(ImageName name, uint16_t x, uint16_t y, uint16_t mode) {
    XpmData* xpm_data = get_xpm_data(name);
    if (xpm_data == NULL) return 1;
    return vg_draw_xpm_from_bottom_left_corner(xpm_data->image, xpm_data->map, x, y, mode);
}
