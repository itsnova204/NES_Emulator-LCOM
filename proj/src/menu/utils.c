#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb)
{
  if (lsb == NULL)
    return 1;
  *lsb = (uint8_t)val;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb)
{
  if (msb == NULL)
    return 1;
  *msb = (uint8_t)(val >> 8);

  return 0;
}

int(util_sys_inb)(int port, uint8_t *value)
{ // leitura; transforma o valor de 32 bits para 8 bits
  if (value == NULL)
    return 1;

  uint32_t temp;
  if (sys_inb(port, &temp) != 0)
    return 1;

  *value = (uint8_t)temp;

  return 0;
}
