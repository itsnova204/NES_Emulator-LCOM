#include "graphics.h"
#include "VBE.h"

static vbe_mode_info_t vbe_mode_info; // modo do video atual
static uint8_t* frame_buffer;         // VRAM - armazenar os valores de cor de cada pixel

int set_graphic_mode(uint16_t mode) {
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));   // alocar memória para a estrutura reg86

  reg86.intno = 0x10;       // BIOS video services
  reg86.ax = VBE_SET_MODE;  // set VBE mode
  reg86.bx = mode | LINEAR_FRAME_BUF; 

  if(sys_int86(&reg86) != 0 || reg86.al != 0x4F || reg86.ah != 0x00) {
    printf("vg_init_mode(): sys_int86() failed \n");
    return -1;
  }

  return 0;
}

int set_frame_buffer(uint16_t mode) {
  if (vbe_get_mode_info(VBE_MODE_INDEXED, &vbe_mode_info) != 0) {
    printf("set_frame_buffer(): vbe_get_mode_info() failed \n");
    return 1;
  }

  int PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel); // bytes por pixel
  int TotalBytes = vbe_mode_info.XResolution * vbe_mode_info.YResolution * PixelColorBytes; // total de bytes para armazenar toda a informacao de cor

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &vbe_mode_info.PhysBasePtr) != 0) {
    printf("set_frame_buffer(): sys_privctl() failed \n");
    return 1;
  } 

  frame_buffer = (uint8_t*) vm_map_phys(SELF, (void*) vbe_mode_info.PhysBasePtr, TotalBytes);

  return (frame_buffer == NULL);
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {

  if (x > vbe_mode_info.XResolution || y > vbe_mode_info.YResolution) {
    printf("vg_draw_pixel(): coordinates exceed resolution \n");
    return 1;
  }

  int PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel); // bytes por pixel


  return 0;
}


int numberOfBytesForBits(int bits) {
  return (bits + 7) / 8;
}