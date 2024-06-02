#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int hook_id = 0;
int count = 0;

int(get_counter)()
{
  return count;
}

int(timer_set_frequency)(uint8_t timer, uint32_t freq)
{
  if (timer > 2 || freq < 19 || freq > TIMER_FREQ)
    return 1;

  uint8_t st;                          // status do timer (1 byte)
  if (timer_get_conf(timer, &st) != 0) // buscar a configuração do timer
    return 1;

  uint8_t control = TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_BIN; // configurar o controlo do timer (modo 3, binário)

  switch (timer)
  { // selecionar o timer
  case 0:
    control |= TIMER_SEL0;
    break;
  case 1:
    control |= TIMER_SEL1;
    break;
  case 2:
    control |= TIMER_SEL2;
    break;
  default:
    return 1;
  }

  if (sys_outb(TIMER_CTRL, control) != OK) // escreve na porta indicada do timer o byte passado de controlo (configurado anteriormente)
    return 1;

  uint16_t div = TIMER_FREQ / freq; // cálculo da frequência

  uint8_t lsb, msb;
  if (util_get_LSB(div, &lsb) != 0)
    return 1;

  if (util_get_MSB(div, &msb) != 0)
    return 1;

  if (sys_outb(TIMER_0 + timer, lsb) != OK)
    return 1;

  if (sys_outb(TIMER_0 + timer, msb) != OK)
    return 1;

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no)
{
  if (bit_no == NULL)
    return 1;

  *bit_no = BIT(hook_id); // levanta a posição do hook_id a 1
  
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK)
    return 1;

  return 0;
}

int(timer_unsubscribe_int)()
{
  if (sys_irqrmpolicy(&hook_id) != OK)
    return 1;

  return 0;
}

void(timer_int_handler)()
{
  count++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st)
{
  if (st == NULL || timer > 2 || timer < 0)
    return 1;

  uint8_t control = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  
  if (sys_outb(TIMER_CTRL, control) != OK)
    return 1;

  if (util_sys_inb(TIMER_0 + timer, st) != OK)
    return 1;

  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{
  if (timer > 2 || timer < 0)
    return 1;

  union timer_status_field_val conf;

  switch (field)
  {
  case tsf_all: // timer_status_field defined in lcom
    conf.byte = st;
    break;

  case tsf_initial:
    conf.in_mode = (st & TIMER_LSB_MSB) >> 4;
    break;

  case tsf_mode:
    conf.count_mode = (st & 0x0E) >> 1;

    if (conf.count_mode == 6)
      conf.count_mode = 2;
    else if (conf.count_mode == 7)
      conf.count_mode = 3;

    break;

  case tsf_base:

    conf.bcd = st & TIMER_BIN;
    break;

  default:
    return 1;
  }

  if (timer_print_config(timer, field, conf) != 0)
    return 1;

  return 0;
}
