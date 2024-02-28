#include <lcom/lcf.h>

#include <stdint.h>

#define 	BIT(n)   (1 << (n))

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;

  uint16_t mask = 0xFF;
  *lsb = mask & val;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  
  *msb = val >> 8;

  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) return 1;

  uint32_t tempVal;
  
  if(sys_inb(port, tempVal) == 1){
    return 1;
  }

  *value = (uint8_t) tempVal;
  
  return 0;
}
