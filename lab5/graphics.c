#include <lcom/lcf.h>

#include <graphics.h>
#include <vbe.h>
#include <utils.h>

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

int set_frame_buffer(uint16_t mode){ //TODO: using panic() instead of printf() feels wrong as we can fall back on text mode no? ask teacher.
  memset(&vbe_mode_info, 0, sizeof(vbe_mode_info)); //reset vbe_mode_info
  if(be_get_mode_info(mode, &vbe_mode_info)){       //get vbe_mode_info
    printf("create_frame_buffer(): vbe_get_mode_info() failed\n");
    return -1;
  }
 
  int r; //variable to store the return value of sys_privctl
  struct minix_mem_range mr;

  unsigned int vram_base = vbe_get_vram_base();
  unsigned int vram_size = vbe_get_vram_size();
  vRES = vbe_get_vres();
  hRES = vbe_get_hres();

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

