#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

//sys_out -> escrever dados numa porta
//sys_int -> ler dados de uma porta

int hook_id = 0;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  //base freq is 1193182 Hz
  //timer has 16 bits, maximum divisor is 65535 (0XFFFF)
  //minimum freq is 1193182/65535 = 18.2 Hz
  if (freq > TIMER_FREQ || freq < (TIMER_FREQ/0xFFFF)) {
    return 1;
  }

  //consultar configuracao atual do timer
  uint8_t controlWord;
  if (timer_get_conf(timer, &controlWord) != 0) return 1;

  // Novo comando de configuração, ativamos os bits da zona 'LSB followed by MSB' e mantemos os restantes
  controlWord = (controlWord & 0x0F) | TIMER_LSB_MSB; 

  //calcular o valor inicial do contador
  uint32_t initialValue = TIMER_FREQ / freq;
  uint8_t MSB, LSB;
  util_get_MSB(initialValue, &MSB);
  util_get_LSB(initialValue, &LSB);

  //atualizar a controlWord de acordo com o timer escolhido
  //selectedTimer possui a porta para o timer escolhido (40h,41h,42h)
  uint8_t selectedTimer;
  switch (timer) {
    case 0:
      controlWord |= TIMER_SEL0;
      selectedTimer = TIMER_0;
      break;
    case 1:
      controlWord |= TIMER_SEL1;
      selectedTimer = TIMER_1;
      break;
    case 2:
      controlWord |= TIMER_SEL2;
      selectedTimer = TIMER_2;
      break;
    default:
      return 1; //não é nenhum dos timers
  }

  //avisar o i8254 que vamos configurar o timer
  if (sys_outb(TIMER_CTRL, controlWord) != 0) return 1;

    // Avisamos o i8254 que vamos configurar o timer
  if (sys_outb(TIMER_CTRL, controlWord) != 0) return 1;

  // Injetamos o valor inicial do contador (lsb seguido de msb) diretamente no registo correspondente
  if (sys_outb(selectedTimer, LSB) != 0) return 1;
  if (sys_outb(selectedTimer, MSB) != 0) return 1;
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if( bit_no == NULL) return 1; // o apontador deve ser válido
  *bit_no = BIT(hook_id);       // a função que chamou esta deve saber qual é a máscara a utilizar
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0) return 1; // subscrição das interrupções
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != 0) return 1; // desligar as interrupções
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL || timer > 2 || timer < 0) return 1;
  uint8_t RBC = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
  if (sys_outb(TIMER_CTRL, RBC) != 0) return 1;
  if (util_sys_inb(TIMER_0 + timer, st)) return 1;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val data;

  switch (field) {

    case tsf_all: 
      data.byte = st; 
      break;

    case tsf_initial:                                       
      st = (st >> 4) & 0x03;

      if (st == 1) data.in_mode = LSB_only;
      else if (st == 2) data.in_mode = MSB_only;
      else if (st == 3) data.in_mode = MSB_after_LSB;
      else data.in_mode = INVAL_val;
      
      break;

    case tsf_mode:
      st = (st >> 1) & 0x07;

      if(st == 6) data.count_mode = 2;
      else if(st == 7) data.count_mode = 3;
      else data.count_mode = st;

      break;
      
    case tsf_base:
      data.bcd = st & TIMER_BCD;
      break;        

    default:
      return 1;
  }

  if (timer_print_config(timer, field, data) != 0) return 1;
  return 0;
}
