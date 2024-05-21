#include "graphics.h"

static vbe_mode_info_t vbe_mode_info;
static uint8_t* frame_buffer; 
static uint8_t* back_buffer;
size_t frame_buffer_size;


size_t numberOfBytesForBits(size_t bits) {
  return (bits + 7) / 8;
}


vbe_mode_info_t (get_vbe_mode_info)() {
  return vbe_mode_info;
}

/**** FUNCTIONS RELATED TO MODE SETUP AND BUFFERS ****/

int set_graphic_mode(uint16_t mode) {
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));   
  reg86.intno = 0x10;

  reg86.ax = VBE_SET_MODE;
  reg86.bx = mode | LINEAR_FRAME_BUF;

  if(sys_int86(&reg86) != 0 || reg86.al != VBE_FUNCTION_INVOKE || reg86.ah != VBE_FUNCTION_SUCCESSFULLY) {
    printf("vg_init_mode(): sys_int86() failed \n");
    return -1;
  }

  return 0;
}


int (set_frame_buffer)(uint16_t mode) {
  if (vbe_get_mode_info(mode, &vbe_mode_info) != 0) {
    printf("set_frame_buffer(): vbe_get_mode_info() failed \n");
    return 1;
  }
  
  size_t PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel); 
  size_t TotalBytes = vbe_mode_info.XResolution * vbe_mode_info.YResolution * PixelColorBytes; 

  frame_buffer_size = TotalBytes;

  struct minix_mem_range memory_range = {
    vbe_mode_info.PhysBasePtr,              // base address
    vbe_mode_info.PhysBasePtr + TotalBytes  // end address
  };

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
  memset(back_buffer, 0, frame_buffer_size);  // Clear the back buffer
}


/**** FUNCTIONS RELATED TO DRAWING ON BUFFER ****/

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if (color == 0xff000000) return 0;
  //printf("%x\n", color);

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


int (vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  uint16_t width = vbe_mode_info.XResolution / no_rectangles;

  uint16_t height = vbe_mode_info.YResolution / no_rectangles;

  uint32_t color;

  for (int i = 0; i < no_rectangles; i++) {
    for (int j = 0; j < no_rectangles; j++) {

      if (vbe_mode_info.MemoryModel == 6) {
        color = Red(j, step, first) << vbe_mode_info.RedFieldPosition | 
                Green(i, step, first) << vbe_mode_info.GreenFieldPosition | 
                Blue(j, i, step, first) << vbe_mode_info.BlueFieldPosition;
      } else {
        color = (first + (i * no_rectangles + j) * step) % (1 << vbe_mode_info.BitsPerPixel);
      }
      
      if (vg_draw_rectangle(j * width, i * height, width, height, color) != 0) {
        printf("vg_draw_pattern(): vg_draw_rectangle() failed \n");
        vg_exit();

        return 1;
      }
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

      uint32_t color = normalizeColor(*(uint32_t*) &colorMap[colorIndex * 4], mode);

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


int vg_draw_xpm_from_bottom_left_corner(xpm_image_t xpm_image, uint8_t *colorMap, uint16_t x, uint16_t y, uint16_t mode) {

  if (colorMap == NULL) {
    printf("vg_draw_xpm(): xpm_load() failed \n");
    vg_exit();

    return 1;
  }

  for (int height = 0; height < xpm_image.height; height++) {
    for (int width = 0; width < xpm_image.width; width++) {

      int colorIndex = (height * xpm_image.width) + width;

      uint32_t color = normalizeColor(*(uint32_t*) &colorMap[colorIndex * 4], mode);

      if (vg_draw_pixel(x + width, y - xpm_image.height + 1 + height, color) != 0) {
        printf("vg_draw_xpm(): vg_draw_pixel() failed \n");
        vg_exit(); 

        return 1;
      }
    }
  }
  
  return 0;
}


int vg_draw_rectangle_from_bottom_left_corner(int16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  for (int i = 0; i < height; i++) {
    if (vg_draw_hline(x, y - height + 1 + i, width, color) != 0) {
      printf("vg_draw_rectangle(): vg_draw_hline() failed \n");
      vg_exit();

      return 1;
    }
  }

  return 0;
}


int vg_draw_xpm_partial(xpm_image_t xpm_image, uint8_t *colorMap, uint16_t x, uint16_t y, uint16_t image_start_x, uint16_t mode) {

  if (colorMap == NULL) {
    printf("vg_draw_xpm(): xpm_load() failed \n");
    vg_exit();
    return 1;
  }

  for (int height = 0; height < xpm_image.height; height++) {
    for (int width = image_start_x; width < xpm_image.width; width++) {

      int colorIndex = (height * xpm_image.width) + width;

      uint32_t color = normalizeColor(*(uint32_t*) &colorMap[colorIndex * 4], mode);

      uint16_t xf = x + width - image_start_x; 

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

/*** COLORS RELATED FUNCTIONS ***/


uint32_t (normalizeColor)(uint32_t color, uint16_t mode) {
  if (mode == VBE_MODE_INDEXED) {
    return color;
  } 
  
  else if (mode == VBE_MODE_DC_15) {
    // 15-bit color mode: ((1:)5:5:5)
    uint32_t r = (color >> 19) & 0x1F; 
    uint32_t g = (color >> 11) & 0x1F;  
    uint32_t b = (color >> 3)  & 0x1F;
    return (r << 10) | (g << 5) | b;
  } 
  
  else if (mode == VBE_MODE_DC_16) {
    // 16-bit color mode: (5:6:5)
    uint32_t r = (color >> 19) & 0x1F; 
    uint32_t g = (color >> 10) & 0x3F;
    uint32_t b = (color >> 3)  & 0x1F;
    return (r << 11) | (g << 5) | b;
  } 
  
  else if (mode == VBE_MODE_DC_24) {
    return color & 0xFFFFFF;
  } 
  
  else if (mode == VBE_MODE_DC_32) {
    // 32-bit color mode: ((8:)8:8:8)
    return color;
  } 
  
  else {
    printf("normalizeColor(): invalid mode \n");

    return 0;
  }
}

uint32_t (Red)(unsigned j, uint8_t step, uint32_t first) {

  return (R(first) + j * step) % (1 << vbe_mode_info.RedMaskSize);
}

uint32_t (Green)(unsigned i, uint8_t step, uint32_t first) {

  return (G(first) + i * step) % (1 << vbe_mode_info.GreenMaskSize);
}

uint32_t (Blue)(unsigned j, unsigned i, uint8_t step, uint32_t first) {

  return (B(first) + (i + j) * step) % (1 << vbe_mode_info.BlueMaskSize);
}

uint32_t (R)(uint32_t first){

  return ((1 << vbe_mode_info.RedMaskSize) - 1) & (first >> vbe_mode_info.RedFieldPosition);
}

uint32_t (G)(uint32_t first){

  return ((1 << vbe_mode_info.GreenMaskSize) - 1) & (first >>vbe_mode_info.GreenFieldPosition);
}

uint32_t (B)(uint32_t first){
  
  return ((1 << vbe_mode_info.BlueMaskSize) - 1) & (first >> vbe_mode_info.BlueFieldPosition);
}

