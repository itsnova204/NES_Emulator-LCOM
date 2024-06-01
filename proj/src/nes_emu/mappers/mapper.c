#include "../mapper.h"

Mapper *MapperCreate(u8 PRGbanks, u8 CHRbanks) {
    Mapper *mapper = (Mapper*)malloc(sizeof(Mapper));
    mapper->PRGbanks = PRGbanks;
    mapper->CHRbanks = CHRbanks;
    mapper->MapperCpuRead = NULL;
    mapper->MapperCpuWrite = NULL;
    mapper->MapperPpuRead = NULL;
    mapper->MapperPpuWrite = NULL;
    return mapper;
}
