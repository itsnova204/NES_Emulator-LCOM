#include <lcom/lcf.h>

#include <src/drivers/graphics.h>
#include <src/drivers/vbe.h>

//Documentation:
//https://web.fe.up.pt/~pfs/aulas/lcom2324/at/8xpm.pdf
//https://web.fe.up.pt/~pfs/aulas/lcom2324/at/7video.pdf
//https://web.fe.up.pt/~pfs/aulas/lcom2223/labs/lab5/lab5_5.html

vbe_mode_info_t vbe_mode_info;

uint16_t vRES = 0;
uint16_t hRES = 0;
void* frame_buffer;

int vg_init_mode(uint16_t mode){  
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));

  reg86.intno = 0x10;
  reg86.ax = VBE_FUNC_SET_MODE;
  reg86.bx = mode | LINEAR_FRAME_BUF;

  if( sys_int86(&reg86) != OK || reg86.al != 0x4F || reg86.ah != 0x00) {
    printf("vg_init_mode(): sys_int86() failed \n");
    return -1;
  }
  return 0;
}

int vg_init_framebuffer(uint16_t mode){ //TODO: using panic() instead of printf() feels wrong as we can fall back on text mode no? ask teacher.
  memset(&vbe_mode_info, 0, sizeof(vbe_mode_info)); //reset vbe_mode_info
  if(vbe_get_mode_info(mode, &vbe_mode_info)){       //get vbe_mode_info
    printf("create_frame_buffer(): vbe_get_mode_info() failed\n");
    return -1;
  }
 
  int r; //variable to store the return value of sys_privctl
  struct minix_mem_range mr;

  unsigned int vram_base = vbe_mode_info.PhysBasePtr;
  unsigned int vram_size = vbe_mode_info.XResolution * vbe_mode_info.YResolution * (vbe_mode_info.BitsPerPixel / 8);

  hRES = vbe_mode_info.XResolution;
  vRES = vbe_mode_info.YResolution;

  mr.mr_base = (phys_bytes)vram_base;
  mr.mr_limit = mr.mr_base + (phys_bytes)vram_size;

  if((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)) != OK){
    printf("DRIVER ERROR [VIDEO]: set framebuffer sys_privctl (ADD_MEM) failed!: %d\n", r);
    return -1;
  }

  frame_buffer = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(frame_buffer == MAP_FAILED){
    printf("DRIVER ERROR [VIDEO]: Couldn't map video memory!\n");
    return -1;
  }

  return 0;
}


int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){
  if(x >= hRES || y >= vRES){//check out of bounds
    printf("DRIVER ERROR [VIDEO]: Tried to draw pixel out of bounds!\n");
    return -1;
  }

  //find position of pixel in frame_buffer
  uint8_t* pixel_ptr = (uint8_t*)frame_buffer + (y * hRES + x) * (vbe_mode_info.BitsPerPixel / 8);

  //draw pixel
  if (memcpy(pixel_ptr, &color, vbe_mode_info.BitsPerPixel/8) == NULL){
    printf("DRIVER ERROR [VIDEO]: Drawing pixel failed!\n");
    return -1;
  }

  return 0;
}

int load_externFrameBuffer(void* buffer){
  if (buffer == NULL){
    printf("DRIVER ERROR [VIDEO]: Tried to load null frame buffer!\n");
    return -1;
  }
  
  frame_buffer = buffer;
  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){ //draws a horizontal line
  if(x >= hRES || y >= vRES){//check out of bounds
    printf("DRIVER ERROR [VIDEO]: Tried to draw line out of bounds!\n");
    return -1;
  }
  
  for (uint16_t i = 0 ; i < len ; i++) {
    if(vg_draw_pixel(x + i,y,color) != 0) {
      printf("DRIVER ERROR [VIDEO]: Drawing hline failed!\n");
      return -1;
    }
  }

  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  if(x >= hRES || y >= vRES){//check out of bounds
    printf("DRIVER ERROR [VIDEO]: Tried to draw rectangle out of bounds!\n");
    return -1;
  }

  for (uint16_t i = 0 ; i < height ; i++) {
    if(vg_draw_hline(x,y + i, width, color) != 0) {
      printf("DRIVER ERROR [VIDEO]: Drawing rectangle failed!\n");
      return -1;
    }
  }

  return 0;
}
