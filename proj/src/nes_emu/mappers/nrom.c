#include "../mapper.h"

bool MapperCpuRead(Mapper *mapper, u16 addr, u32* mappedAddr);
bool MapperCpuWrite(Mapper *mapper, u16 addr, u32* mappedAddr);

bool MapperPpuRead(Mapper *mapper, u16 addr, u32* mappedAddr);
bool MapperPpuWrite(Mapper *mapper, u16 addr, u32* mappedAddr);

void MapperLoadNROM(Mapper *mapper) {
    mapper->MapperCpuRead = MapperCpuRead;
    mapper->MapperCpuWrite = MapperCpuWrite;
    mapper->MapperPpuRead = MapperPpuRead;
    mapper->MapperPpuWrite = MapperPpuWrite;
}

bool MapperCpuRead(Mapper *mapper, u16 addr, u32* mappedAddr) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        *mappedAddr = addr & (mapper->PRGbanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool MapperCpuWrite(Mapper *mapper, u16 addr, u32* mappedAddr) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        *mappedAddr = addr & (mapper->PRGbanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool MapperPpuRead(Mapper *mapper, u16 addr, u32* mappedAddr) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        *mappedAddr = addr;
        return true;
    }
    return false;
}

bool MapperPpuWrite(Mapper *mapper, u16 addr, u32* mappedAddr) {
    // if (addr >= 0x0000 && addr <= 0x1FFF) {
    //     if (mapper->CHRbanks == 0) {
    //         // Treat as RAM
    //         *mappedAddr = addr;
    //         return true;
    //     }
    // }
    return false;
}
