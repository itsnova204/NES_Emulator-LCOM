#include <lcom/lcf.h>

#include <stdint.h>

//val -> valor de entrada com 16 bits (2 bytes)
//lsb -> byte menos significativo (ultimo byte) [LEAST SIGNIFICANT BYTE]
//OxFF -> máscara para tornar zero todos os bytes exceto o lsb
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = (uint8_t) (val & 0xFF);
  return 0;
}

//msb -> [MOST SIGNIFICANT BYTE]
int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = (uint8_t) (val >> 8);
  return 0;
}

//Leitura do valor que está no 'port' e armazenar no 'value'
//INT -> quantidade de bits é sempre igual à arquitetura
int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) return 1;
  uint32_t val;
  int ret = sys_inb(port, &val);  
  *value = val & 0xFF;
  return ret;
}
