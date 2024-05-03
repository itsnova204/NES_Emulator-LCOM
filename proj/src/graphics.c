#include "graphics.h"
#include "VBE.h"

static vbe_mode_info_t vbe_mode_info; // informacao do modo de video atual
static uint8_t* frame_buffer;         // VRAM - armazenar os valores de cor de cada pixel
static uint8_t* back_buffer;
size_t frame_buffer_size;

size_t numberOfBytesForBits(size_t bits) {
  return (bits + 7) / 8;
}

vbe_mode_info_t (get_vbe_mode_info)() {
  return vbe_mode_info;
}

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

  frame_buffer = (uint8_t*) vm_map_phys(SELF, (void*) vbe_mode_info.PhysBasePtr, TotalBytes);
  if (frame_buffer == MAP_FAILED) {
    printf("set_frame_buffer(): vm_map_phys() failed \n");
    return 1;
  }

  back_buffer = (uint8_t*) malloc(TotalBytes);
  if (back_buffer == NULL) {
    printf("set_frame_buffer(): malloc() failed \n");
    return 1;
  }

  return 0;
}

void swap_buffers() {
  memcpy(frame_buffer, back_buffer, frame_buffer_size);
  memset(back_buffer, 0, frame_buffer_size);
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (x >= vbe_mode_info.XResolution || y >= vbe_mode_info.YResolution) {
    printf("vg_draw_pixel(): coordinates exceed resolution \n");
    //return 1;
    return 0;
  }

  int PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel);
  uint8_t* pixel = back_buffer + (y * vbe_mode_info.XResolution + x) * PixelColorBytes;

  memcpy(pixel, &color, PixelColorBytes);

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

int vg_draw_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
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

uint32_t normalizeColor(uint32_t color, uint16_t mode) {
  if (mode == VBE_MODE_INDEXED) {
    return color;
  } else if (mode == VBE_MODE_DC_15) {
    return ((color & 0x1F) << 3) | ((color & 0x3E0) << 6) | ((color & 0x7C00) << 9);
  } else if (mode == VBE_MODE_DC_16) {
    return ((color & 0x1F) << 3) | ((color & 0x7E0) << 5) | ((color & 0xF800) << 8);
  } else if (mode == VBE_MODE_DC_24) {
    return color;
  } else if (mode == VBE_MODE_DC_32) {
    return color;
  } else {
    printf("normalizeColor(): invalid mode \n");
    return 0;
  }
}

int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y, uint16_t mode) {
  xpm_image_t xpm_image;

  switch(mode) {
    case VBE_MODE_INDEXED:
      xpm_image.type = XPM_INDEXED;
      break;
    case VBE_MODE_DC_15:
      xpm_image.type = XPM_1_5_5_5;
      break;
    case VBE_MODE_DC_16:
      xpm_image.type = XPM_5_6_5;
      break;
    case VBE_MODE_DC_24:
      xpm_image.type = XPM_8_8_8;
      break;
    case VBE_MODE_DC_32:
      xpm_image.type = XPM_8_8_8_8;
      break;
    default:
      printf("vg_draw_xpm(): invalid mode \n");
      return 1;
  }
  
  uint8_t *colorMap = xpm_load(xpm, xpm_image.type, &xpm_image);

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

int vg_clear_screen() {
  if (vg_draw_rectangle(0, 0, vbe_mode_info.XResolution, vbe_mode_info.YResolution, 0) != 0) {
    printf("vg_clear_screen(): vg_draw_rectangle() failed \n");
    vg_exit();
    return 1;
  }
  
  return 0;
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

int vg_draw_xpm_from_bottom_left_corner(xpm_map_t xpm, uint16_t x, uint16_t y, uint16_t mode) {
  xpm_image_t xpm_image;

  switch(mode) {
    case VBE_MODE_INDEXED:
      xpm_image.type = XPM_INDEXED;
      break;
    case VBE_MODE_DC_15:
      xpm_image.type = XPM_1_5_5_5;
      break;
    case VBE_MODE_DC_16:
      xpm_image.type = XPM_5_6_5;
      break;
    case VBE_MODE_DC_24:
      xpm_image.type = XPM_8_8_8;
      break;
    case VBE_MODE_DC_32:
      xpm_image.type = XPM_8_8_8_8;
      break;
    default:
      printf("vg_draw_xpm(): invalid mode \n");
      return 1;
  }
  
  uint8_t *colorMap = xpm_load(xpm, xpm_image.type, &xpm_image);

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

int vg_draw_rectangle_from_bottom_left_corner(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (int i = 0; i < height; i++) {
    if (vg_draw_hline(x, y - height + 1 + i, width, color) != 0) {
      printf("vg_draw_rectangle(): vg_draw_hline() failed \n");
      vg_exit();
      return 1;
    }
  }

  return 0;
}

int vg_draw_xpm_partial(xpm_map_t xpm, uint16_t x, uint16_t y, uint16_t image_start_x, uint16_t mode) {
  xpm_image_t xpm_image;

  switch(mode) {
    case VBE_MODE_INDEXED:
      xpm_image.type = XPM_INDEXED;
      break;
    case VBE_MODE_DC_15:
      xpm_image.type = XPM_1_5_5_5;
      break;
    case VBE_MODE_DC_16:
      xpm_image.type = XPM_5_6_5;
      break;
    case VBE_MODE_DC_24:
      xpm_image.type = XPM_8_8_8;
      break;
    case VBE_MODE_DC_32:
      xpm_image.type = XPM_8_8_8_8;
      break;
    default:
      printf("vg_draw_xpm(): invalid mode \n");
      return 1;
  }
  
  uint8_t *colorMap = xpm_load(xpm, xpm_image.type, &xpm_image);

  if (colorMap == NULL) {
    printf("vg_draw_xpm(): xpm_load() failed \n");
    vg_exit();
    return 1;
  }
  for (int height = 0; height < xpm_image.height; height++) {
    for (int width = image_start_x; width < xpm_image.width; width++) {
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
