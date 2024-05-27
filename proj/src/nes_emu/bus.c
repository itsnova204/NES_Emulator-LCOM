#include "bus.h"

#include "p6502.h"
#include "ppu.h"
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

uint8_t dma_page = 0x00;
uint8_t dma_addr = 0x00;
uint8_t dma_data = 0x00;

bool dma_dummy = true;
bool dma_transfer = false;

bool ppu_nmi;

uint8_t* OAM_ptr;

FILE* fp2;
int bus_init(char* cart_filePath){
  memset(&sys_ram, 0, 2048); //nes has 2kb of ram (0x0000 - 0x07FF)
  printf("[BUS] Starting rom: %s\n", cart_filePath);
  //start sys_ram with 0;
  memset(&sys_ram, 0, 2048); //nes has 2kb of ram (0x0000 - 0x07FF)

  //controler_init(controller);

  if (cart_insert(cart_filePath))return 1;
  
  cpu_reset();
  ppu_init();
  OAM_ptr = getOAM_ptr();

  return 0;
}

int bus_exit(){
  cart_remove();
  ppu_exit();
  return 0;
}


void bus_clock(){
  ppu_nmi=ppu_clock();


if (mainClockCounter % 3 == 0){
	if (dma_transfer){
		if (dma_dummy){
			if (mainClockCounter % 2 == 1){
				dma_dummy = false;
			}
		}else{
			if (mainClockCounter % 2 == 0)
			{
				dma_data = sysBus_read(dma_page << 8 | dma_addr);
			}
			else
			{
				OAM_ptr[dma_addr] = dma_data;
				dma_addr++;
				if (dma_addr == 0x00)
				{
					dma_transfer = false;
					dma_dummy = true;
				}
			}
		}
	}else{
     	//no DMA, proceed as normal
		cpu_clock();
	}		
}
  if (ppu_nmi){
    //printf("nmi triggered\n");
    ppu_disable_nmi();
		cpu_nmi();
  }   

	mainClockCounter++;
}
int readcounter = 0;
uint8_t sysBus_read(uint16_t addr) {

    uint8_t data = 0x00;
    bool hijack = false;
    //uint16_t oldaddr = addr;

    data = sys_readFromCard(addr, &hijack);
    if(hijack){
        addr = 0xffff;
    }
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        data = sys_ram[addr & 0x07FF];
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        data = cpuBus_readPPU(addr & 0x0007);
    }else if (addr >= 0x4016 && addr <= 0x4017) {
        data = (controller_state[addr & 0x0001] & 0x80) > 0;
        controller_state[addr & 0x0001] <<= 1;
    }


    //printf("read %02x, from %04x n%6d\n",data,oldaddr,readcounter++ );
    return data;
}

int writeCounter = 0;

void sysBus_write(uint16_t addr, uint8_t data) {
    bool hijack = false;
   //printf("write %02x, to %04x n%6d\n",data,addr,writeCounter++ );

    sys_writeToCard(addr, data, &hijack);
    if(hijack) return;
    else if (addr <= 0x1FFF) {
        sys_ram[addr & 0x07FF] = data;
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        cpuBus_writePPU(addr & 0x0007, data);
    }	else if (addr == 0x4014){
		// DMA transfer https://www.nesdev.org/wiki/DMA
      dma_page = data;
      dma_addr = 0x00;
      dma_transfer = true;						
	  }
    else if (addr >= 0x4016 && addr <= 0x4017) {
        controller_state[addr & 0x0001] = controller[addr & 0x0001];
    }
}
