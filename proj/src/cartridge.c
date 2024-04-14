#include "cartridge.h"

#include "mapper.h"
#include "bus.h"

static uint8_t mapper_id = 0;
static uint8_t nPRG_membanks = 0;
static uint8_t nCHR_membanks = 0;

uint8_t* PRGmem = NULL;
uint8_t* CHRmem = NULL;

//https://www.nesdev.org/wiki/INES
//https://www.nesdev.org/wiki/Nintendo_header
//https://bheisler.github.io/post/nes-rom-parser-with-nom/

struct cart_header { 
		char name[4];
		uint8_t nPRG_rom_chunks;
		uint8_t nCHR_rom_chunks;
		uint8_t mapperLsb_andFlags;
		uint8_t mapperMsb_andFlags;
		uint8_t prg_ram_size;
		uint8_t tv_system1;
		uint8_t tv_system2;
		char unused[5];
	} header;

enum MIRROR
{
    HORIZONTAL,
    VERTICAL,
    ONESCREEN_LO,
    ONESCREEN_HI,
  } mirror_type;

enum ROM_FORMAT {
    NES,
    INES,
    NES2,
  } rom_format = INES;

int cart_insert(){//TODO: implement dynamic rom path
  //read header
  FILE *fp = fopen("./Super_mario_brothers.nes", "rb");
  if (fp == NULL){
    printf("Error: Could not open rom\n");
    return 1;
  }

  printf("LOADING STARTED!:\n");
  header_parse(fp);

  //TODO: implement rom format detection ROM_FORMAT
  switch (rom_format){
  case NES:
    /* code */
    break;
  case INES:
    if(ines_parse(fp)){
      return 1;
    }
    break;
  case NES2:
    /* code */
    break;

  default:
    break;
  }

  return fclose(fp);
}

void cart_remove(){
  mapper_id = 0;
  nPRG_membanks = 0;
  nCHR_membanks = 0;

  free(PRGmem);
  free(CHRmem);
}

void header_parse(FILE *fp){
  fread(&header, sizeof(struct cart_header), 1, fp);
  
  if (header.mapperLsb_andFlags & 0x04){ //if trainer is present, skip it
    fseek(fp, 512, SEEK_SET);
  }

  mapper_id = (header.mapperMsb_andFlags & 0xF0) | (header.mapperLsb_andFlags >> 4);
  mirror_type = (header.mapperLsb_andFlags & 0x01) ? VERTICAL : HORIZONTAL;

  print_header();
  printf("\nmapper_id: %d\n", mapper_id);

}

uint8_t ines_parse(FILE *fp){
  printf("\nSTARTED iNES PARSER\n");

  
  nPRG_membanks = header.nPRG_rom_chunks;
  printf("nPRG_membanks: %d\n", nPRG_membanks);
  PRGmem = (uint8_t *)malloc(nPRG_membanks * 16384);
  if (PRGmem == NULL){
    printf("PRGmem: failed loading!\n");
    return 1;
  }
  
  nCHR_membanks = header.nCHR_rom_chunks;
  printf("nCHR_membanks: %d\n", nCHR_membanks);
  CHRmem = (uint8_t *)malloc(nCHR_membanks * 8192);
  if (CHRmem == NULL){
    printf("CHRmem: failed loading!\n");
    return 1;
  }

  printf("Memory allocation complete!\n");

  uint8_t PRGmem_read = 0;
  uint8_t CHRmem_read = 0;

  PRGmem_read = fread(PRGmem, 16384, nPRG_membanks, fp);
  CHRmem_read = fread(CHRmem, 8192, nCHR_membanks, fp);

  printf("Loaded PRGmem: %d bytes\n", PRGmem_read*16384);
  printf("Loaded CHRmem: %d bytes\n", CHRmem_read*8192);

  return 0;
}

void print_header(){
  printf("Header:\n");
  printf("Name: %c%c%c%c\n", header.name[0], header.name[1], header.name[2], header.name[3]);
  printf("nPRG_rom_chunks: %d\n", header.nPRG_rom_chunks);
  printf("nCHR_rom_chunks: %d\n", header.nCHR_rom_chunks);
  printf("mapperLsb_andFlags: 0x%02x\n", header.mapperLsb_andFlags);
  printf("mapperMsb_andFlags: 0x%02x\n", header.mapperMsb_andFlags);
  printf("prg_ram_size: %d\n", header.prg_ram_size);
  printf("tv_system1: %d\n", header.tv_system1);
  printf("tv_system2: %d\n", header.tv_system2);
  printf("unused: %c%c%c%c%c\n", header.unused[0], header.unused[1], header.unused[2], header.unused[3], header.unused[4]);
}

uint8_t sys_readFromCard(uint16_t addr, bool* hijack){
  return PRGmem[mapper_map(addr, type_sysBus_read, hijack)];
}

void sys_writeToCard(uint16_t addr, uint8_t data, bool* hijack){
  if (addr >= 0x8000){ //no need to check upper bound, addr data type cant go avobe 0xFFFF
    PRGmem[mapper_map(addr, type_sysBus_write, hijack)] = data;
  }
}

//TODO: this is a ppu function, why is it here!
uint8_t ppu_readFromCard(uint16_t addr, bool* hijack){
  if (addr <= 0x1FFF){
    return CHRmem[mapper_map(addr, type_ppuBus_read_bus, hijack)];
  }
  return 0;
}

//TODO: this is a ppu function, why is it here!
void ppu_writeToCard(uint16_t addr, uint8_t data, bool* hijack){
  if (addr <= 0x1FFF){
    CHRmem[mapper_map(addr, type_ppuBus_write, hijack)] = data;
  }
}

