#include "graphics.h"
#include "VBE.h"

static vbe_mode_info_t vbe_mode_info; // informacao do modo de video atual
static uint8_t* frame_buffer;         // VRAM - armazenar os valores de cor de cada pixel

size_t numberOfBytesForBits(size_t bits) {
  return (bits + 7) / 8;
}

vbe_mode_info_t (get_vbe_mode_info)() {
  return vbe_mode_info;
}

// ativar modo grafico
int set_graphic_mode(uint16_t mode) {
  reg86_t reg86;

  // alocar memória para a estrutura reg86
  memset(&reg86, 0, sizeof(reg86));   

  // numero da interrupcao para a BIOS video services
  reg86.intno = 0x10;

  reg86.ax = VBE_SET_MODE;  // set VBE mode
  reg86.bx = mode | LINEAR_FRAME_BUF;  // set mode & linear frame buffer 

  // reg86.al -> registo pós interrupcao para verificar se o modo foi ativado com sucesso
  // reg86.ah -> registo pós interrupcao para verificar se hardware suporta o modo
  if(sys_int86(&reg86) != 0 || reg86.al != VBE_FUNCTION_INVOKE || reg86.ah != VBE_FUNCTION_SUCCESSFULLY) {
    printf("vg_init_mode(): sys_int86() failed \n");
    return -1;
  }

  return 0;
}

int (set_frame_buffer)(uint16_t mode) {
  // obter informacao do modo que fica guardada em $vbe_mode_info
  if (vbe_get_mode_info(mode, &vbe_mode_info) != 0) {
    printf("set_frame_buffer(): vbe_get_mode_info() failed \n");
    return 1;
  }
  
  // calcular o numero de bytes necessarios para armazenar a cor de cada pixel
  size_t PixelColorBytes = numberOfBytesForBits(vbe_mode_info.BitsPerPixel); 

  // numero total de bytes para alocar no buffer
  size_t TotalBytes = vbe_mode_info.XResolution * vbe_mode_info.YResolution * PixelColorBytes; 

  // intervalo de memoria para alocar o buffer
  struct minix_mem_range memory_range = {
    vbe_mode_info.PhysBasePtr,              // base address
    vbe_mode_info.PhysBasePtr + TotalBytes  // end address
  }; // representa um range [initial, final] na memoria VRAM

  // aloca o buffer na memoria fisica
  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &memory_range) != 0) {
    printf("set_frame_buffer(): sys_privctl() failed \n");
    return 1;
  } 

  // mapear o buffer virtual (ponteiro) para a memoria fisica
  frame_buffer = (uint8_t*) vm_map_phys(SELF, (void*) vbe_mode_info.PhysBasePtr, TotalBytes);

  // retornar 0 se alocacao falhar
  return (frame_buffer == NULL);
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  // verificar se as coordenadas estao dentro da resolucao
  if (x > vbe_mode_info.XResolution || y > vbe_mode_info.YResolution) {
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

// desenhar pattern de retangulos
int vg_draw_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  // calcular o tamanho de cada retangulo
  uint16_t width = vbe_mode_info.XResolution / no_rectangles;
  uint16_t height = vbe_mode_info.YResolution / no_rectangles;

  uint32_t color;
  // iterar para desenhar cada retangulo
  for (int i = 0; i < no_rectangles; i++) {
    for (int j = 0; j < no_rectangles; j++) {

      if (vbe_mode_info.MemoryModel == 6) {
        color = Red(j, step, first) << vbe_mode_info.RedFieldPosition | 
                Green(i, step, first) << vbe_mode_info.GreenFieldPosition | 
                Blue(j, i, step, first) << vbe_mode_info.BlueFieldPosition;
      } else {
        // cor do retangulo
        // first -> cor inicial (base)
        // step -> incremento da cor (soma com a cor base)
        // vbe_mode_info.BitsPerPixel -> numero de bits por pixel
        // (1 << vbe_mode_info.BitsPerPixel) -> numero de cores possiveis
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

// desenhar uma imagem XPM
int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y) {
  xpm_image_t xpm_image;

  // carregar as cores do xpm num mapa
  uint8_t *colors = xpm_load(xpm, XPM_INDEXED, &xpm_image);

  if (colors == NULL) {
    printf("vg_draw_xpm(): xpm_load() failed \n");
    return 1;
  }

  // Iterar sobre a imagem XPM
  // Percorrer todos os pixeis da imagem XPM
  for (int height = 0; height < xpm_image.height; height++) {
    for (int width = 0; width < xpm_image.width; width++) {

      // Calcular o índice do pixel atual na matriz de cores
      // Cor do pixel -> colors[index] (second element no mapa de cores em que a key=index)
      int index = (height * xpm_image.width) + width;

      if (vg_draw_pixel(x + width, y + height, colors[index]) != 0) {
        printf("vg_draw_xpm(): vg_draw_pixel() failed \n");
        vg_exit(); 
        return 1;
      }
    }
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
