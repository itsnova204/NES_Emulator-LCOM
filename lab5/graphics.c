#include <lcom/lcf.h>

#include <graphics.h>
#include <vbe.h>
#include <utils.h>

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


