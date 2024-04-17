#include "graphics.h"
#include "VBE.h"

static vbe_mode_info_t vbe_mode_info; // modo do video atual
static uint8_t* frame_buffer;         // VRAM - armazenar os valores de cor de cada pixel

size_t numberOfBytesForBits(size_t bits) {
  return (bits + 7) / 8;
}

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

int (set_frame_buffer)(uint16_t mode) {
  // obter informacao do modo de video
  if (vbe_get_mode_info(VBE_MODE_INDEXED, &vbe_mode_info) != 0) {
    printf("set_frame_buffer(): vbe_get_mode_info() failed \n");
    return 1;
  }

  size_t PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel); // bytes por pixel
  size_t TotalBytes = vbe_mode_info.XResolution * vbe_mode_info.YResolution * PixelColorBytes; // total de bytes para armazenar toda a informacao de cor

  // range do endereco fisico da memoria
  struct minix_mem_range memory_range = {
    vbe_mode_info.PhysBasePtr,              // base address
    vbe_mode_info.PhysBasePtr + TotalBytes  // end address
  };

  // alocacao da memoria fisica
  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &memory_range) != 0) {
    printf("set_frame_buffer(): sys_privctl() failed \n");
    return 1;
  } 

  // mapear a memoria fisica para a memoria virtual
  frame_buffer = (uint8_t*) vm_map_phys(SELF, (void*) vbe_mode_info.PhysBasePtr, TotalBytes);

  return (frame_buffer == NULL);
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  // verificar se as coordenadas estao dentro da resolucao
  if (x > vbe_mode_info.XResolution || x < 0 || y > vbe_mode_info.YResolution || y < 0) {
    printf("vg_draw_pixel(): coordinates exceed resolution \n");
    return 1;
  }

  int PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel); // bytes por pixel

  uint8_t* pixel = frame_buffer + (y * vbe_mode_info.XResolution + x) * PixelColorBytes;  // posicao do pixel

  if (memcpy(pixel, &color, PixelColorBytes) == NULL) {
    printf("vg_draw_pixel(): memccpy() failed \n");
    return 1;
  }

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

