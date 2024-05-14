#include "bus.h"

#include "p6502.h"
//#include "ppu.h"
#include "cartridge.h"
#include "controler.h"

//The bus is the main fabric connecting all the components of the NES together.
//It is responsible for routing data between the CPU, PPU, and cartridge.
//The bus also contains the system RAM, which is used by the CPU for general purpose memory storage.
//The bus is clocked by the main system clock, which is used to synchronize the operation of all the components.

uint8_t controller[2] = {0, 0}; //controller 1 and 2
uint8_t controller_state[2] = {0, 0}; 

static uint32_t mainClockCounter = 0;
static uint8_t sys_ram[2 * 1024];

void bus_init(char* cart_filePath){
   printf("[BUS] Starting rom: %s\n", cart_filePath);
  //start sys_ram with 0;
  memset(&sys_ram, 0, 2048); //nes has 2kb of ram (0x0000 - 0x07FF)

  controler_init(controller);

  //TODO init ppu and cart
  cart_insert(cart_filePath);
  cpu_init();
}

int bus_exit(){
  cart_remove();
  //ppu_exit();

  return 0;
}

void bus_clock(){
  //ppu_clock();

  if (mainClockCounter % 3 == 0){
		cpu_clock();
	}

	mainClockCounter++;
}

uint8_t sysBus_read(uint16_t addr){
  uint16_t data;
  bool hijack = false;

  data = sys_readFromCard(addr, &hijack);
  if(hijack) return data;

  if (addr <= 0x1FFF){ //access sys_ram
    printf("reading sys_ram!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return sys_ram[addr & 0x07FF];
  }
  if (addr >= 0x2000 && addr <= 0x3FFF){ //access ppu registers
    //return sys_readFromPPU(addr & 0x0007);
  }else if (addr == 0x4016 || addr == 0x4017){ //controllers
    data = (controller_state[addr & 0x0001] & 0x80) > 0;
    controller_state[addr & 0x0001] <<= 1;
  }

  return data;
}

void sysBus_write(uint16_t addr, uint8_t data){
  bool hijack = false;

  sys_writeToCard(addr, data, &hijack);
  if(hijack) return;
  
  if (addr >= 0x8000 && addr <= 0x1FFF){ //access sys_ram
    printf("Writing to sys_ram!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    sys_ram[addr & 0x07FF] = data;

  }else if (addr >= 0x2000 && addr <= 0x3FFF){ //access ppu registers
    //sys_writeToPPU(addr, data);
  }else if (addr == 0x4016 || addr == 0x4017){ //controller 1
    controller_state[addr & 0x0001] = controller[addr & 0x0001]; //update controller state
  }
}
