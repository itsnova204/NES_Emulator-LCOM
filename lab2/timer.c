#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

//sys_out -> escrever dados numa porta
//sys_int -> ler dados de uma porta

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

  // Injetar o valor inicial do contador (lsb seguido de msb) diretamente no registo correspondente
  if (sys_outb(selectedTimer, LSB) != 0) return 1;
  if (sys_outb(selectedTimer, MSB) != 0) return 1;
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if( bit_no == NULL) return 1;   // validar o apontador
  //todo
  return 1;
}

int (timer_unsubscribe_int)() {
  //todo
  return 1;
}

void (timer_int_handler)() {
  //todo
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL || timer > 2 || timer < 0) return 1; //validar o timer e o st

  //todo
  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  //todo
  return 1;
}
