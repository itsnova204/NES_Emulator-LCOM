#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"

int(kbc_read_output)(uint8_t port, uint8_t* output);
int(kbc_read_status)(uint8_t* status);
int(kbc_write_command)(uint8_t port, uint8_t command);


#endif
