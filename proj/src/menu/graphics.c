#include "graphics.h"

static vbe_mode_info_t vbe_mode_info;
static uint8_t* frame_buffer; 
static uint8_t* back_buffer;
size_t frame_buffer_size;

vbe_mode_info_t (get_vbe_mode_info)() {
  return vbe_mode_info;
}

/**** FUNCTIONS RELATED TO MODE SETUP AND BUFFERS ****/

int (set_graphic_mode)(uint16_t mode) {

  reg86_t reg86;

  memset(&reg86, 0, sizeof(reg86));   

  reg86.intno = 0x10;

  reg86.ax = VBE_SET_MODE;

  reg86.bx = mode | LINEAR_FRAME_BUF;

  if(sys_int86(&reg86) != 0 || reg86.al != VBE_FUNCTION_INVOKE || reg86.ah != VBE_FUNCTION_SUCCESSFULLY) {
    printf("vg_init_mode(): sys_int86() failed \n");

    return 1;
  }

  return 0;
}


int (set_frame_buffer)(uint16_t mode) {

  if (vbe_get_mode_info(mode, &vbe_mode_info) != 0) {
    printf("set_frame_buffer(): vbe_get_mode_info() failed \n");

    return 1;
  }

  // Calculate the size of the frame buffer
  size_t PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel); 
  size_t TotalBytes = vbe_mode_info.XResolution * vbe_mode_info.YResolution * PixelColorBytes; 

  frame_buffer_size = TotalBytes;

  struct minix_mem_range memory_range = { vbe_mode_info.PhysBasePtr, vbe_mode_info.PhysBasePtr + TotalBytes };

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &memory_range) != 0) {
    printf("set_frame_buffer(): sys_privctl() failed \n");

    return 1;
  } 


  // Map the physical address to a virtual address
  frame_buffer = (uint8_t*) vm_map_phys(SELF, (void*) vbe_mode_info.PhysBasePtr, TotalBytes);

  if (frame_buffer == MAP_FAILED) {
    printf("set_frame_buffer(): vm_map_phys() failed \n");

    return 1;
  }


  // Allocate memory for the back buffer
  back_buffer = (uint8_t*) malloc(TotalBytes);

  if (back_buffer == NULL) {
    printf("set_frame_buffer(): malloc() failed \n");

    return 1;
  }

  return 0;
}


void (swap_buffers)() {

  memcpy(frame_buffer, back_buffer, frame_buffer_size);

  memset(back_buffer, 0, frame_buffer_size);    // Clear the back buffer

}


/**** FUNCTIONS RELATED TO DRAWING ON BUFFER ****/

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

  //printf("%x\n", color);

  // No not draw the "None" color
  if (color == 0xff000000) return 0;

  if (x >= vbe_mode_info.XResolution || y >= vbe_mode_info.YResolution) {
    printf("vg_draw_pixel(): coordinates exceed resolution \n");

    return 0;
  }

  int PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel);

  uint8_t* pixel = back_buffer + (y * vbe_mode_info.XResolution + x) * PixelColorBytes;

  memcpy(pixel, &color ,PixelColorBytes);

  return 0;
}


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {

  for (int i = 0; i < len; i++) {

    if (vg_draw_pixel(x + i, y, color) != 0) {
      printf("vg_draw_hline(): vg_draw_pixel() failed \n");
      vg_exit();

      return 1;
    }

  }

  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  for (int i = 0; i < height; i++) {

    if (vg_draw_hline(x, y + i, width, color) != 0) {
      printf("vg_draw_rectangle(): vg_draw_hline() failed \n");
      vg_exit();

      return 1;
    }

  }

  return 0;
}

int (vg_draw_xpm)(xpm_image_t xpm_image, uint8_t *colorMap, uint16_t x, uint16_t y, uint16_t mode) {

  if (colorMap == NULL) {
    printf("vg_draw_xpm(): xpm_load() failed \n");
    vg_exit();

    return 1;
  }

  for (int height = 0; height < xpm_image.height; height++) {

    for (int width = 0; width < xpm_image.width; width++) {

      int colorIndex = (height * xpm_image.width) + width;

      uint32_t color = *(uint32_t*) &colorMap[colorIndex * 4];

      uint16_t xf = x + width; 

      if (xf >= vbe_mode_info.XResolution || x < 0) {
        break;
      }

      if (vg_draw_pixel(xf, y + height, color) != 0) {
        printf("vg_draw_xpm(): vg_draw_pixel() failed \n");
        vg_exit(); 

        return 1;
      }
    }

  }
  
  return 0;
}


int (vg_clear_screen)() {

  memset(back_buffer, 0, frame_buffer_size);  // Clear the back buffer
  
  return 0;
}


int (vg_draw_xpm_from_bottom_left_corner)(xpm_image_t xpm_image, uint8_t *colorMap, uint16_t x, uint16_t y, uint16_t mode) {

  if (colorMap == NULL) {
    printf("vg_draw_xpm(): xpm_load() failed \n");
    vg_exit();

    return 1;
  }

  for (int height = 0; height < xpm_image.height; height++) {

    for (int width = 0; width < xpm_image.width; width++) {

      int colorIndex = (height * xpm_image.width) + width;

      uint32_t color = *(uint32_t*) &colorMap[colorIndex * 4];

      if (vg_draw_pixel(x + width, y - xpm_image.height + 1 + height, color) != 0) {
        printf("vg_draw_xpm(): vg_draw_pixel() failed \n");
        vg_exit(); 

        return 1;
      }
    }

  }
  
  return 0;
}


int (vg_draw_rectangle_from_bottom_left_corner)(int16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  for (int i = 0; i < height; i++) {

    if (vg_draw_hline(x, y - height + 1 + i, width, color) != 0) {
      printf("vg_draw_rectangle(): vg_draw_hline() failed \n");
      vg_exit();

      return 1;
    }

  }

  return 0;
}

int (vg_draw_rectangle_border)(int16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t border_width, uint32_t color) {

    int16_t start_x = x - border_width;
    int16_t start_y = y - border_width;
    uint16_t total_width = width + 2 * border_width;
    uint16_t total_height = height + 2 * border_width;

    for (int i = 0; i < border_width; i++) {

        // Top border
        if (vg_draw_hline(start_x, start_y + i, total_width, color) != 0) {
            printf("vg_draw_rectangle_border(): vg_draw_hline() failed\n");
            vg_exit();

            return 1;
        }

        // Bottom border
        if (vg_draw_hline(start_x, start_y + total_height - border_width + i, total_width, color) != 0) {
            printf("vg_draw_rectangle_border(): vg_draw_hline() failed\n");
            vg_exit();

            return 1;
        }

    }

    for (int i = 0; i < total_height; i++) {

        // Left border
        for (int j = 0; j < border_width; j++) {

            if (vg_draw_pixel(start_x + j, start_y + i, color) != 0) {
                printf("vg_draw_rectangle_border(): vg_draw_pixel() failed\n");
                vg_exit();

                return 1;
            }

        }
        
        // Right border
        for (int j = total_width - border_width; j < total_width; j++) {

            if (vg_draw_pixel(start_x + j, start_y + i, color) != 0) {
                printf("vg_draw_rectangle_border(): vg_draw_pixel() failed\n");
                vg_exit();

                return 1;
            }

        }

    }

    return 0;
}

int (vg_draw_xpm_partial)(xpm_image_t xpm_image, uint8_t *colorMap, uint16_t x, uint16_t y, uint16_t image_start_x, uint16_t mode) {

  if (colorMap == NULL) {
    printf("vg_draw_xpm(): xpm_load() failed \n");
    vg_exit();

    return 1;
  }

  for (int height = 0; height < xpm_image.height; height++) {

    for (int width = image_start_x; width < xpm_image.width; width++) {

      int colorIndex = (height * xpm_image.width) + width;

      uint32_t color = *(uint32_t*) &colorMap[colorIndex * 4];

      uint16_t xf = x + width - image_start_x; 

      if (xf >= vbe_mode_info.XResolution || x < 0) break;  // Stop drawing if the image exceeds the screen

      if (vg_draw_pixel(xf, y + height, color) != 0) {
        printf("vg_draw_xpm(): vg_draw_pixel() failed \n");
        vg_exit(); 

        return 1;
      }
    }

  }
  
  return 0;
}

