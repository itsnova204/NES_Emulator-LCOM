#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int hook_id = 0;
static int counter = 0;

int (get_counter)() {
  return counter;
}

//sys_out -> escrever dados numa porta
//sys_int -> ler dados de uma porta

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  //base freq is 1193182 Hz
  //timer has 16 bits, maximum divisor is 65535 (0XFFFF)
  //minimum freq is 1193182/65535 = 18.2 Hz
  if (freq > TIMER_FREQ || freq < 19 || timer > 2) {
    return 1;
  }

  //Control Word-> palavra de controlo usado para configurar o timer
  //consultar configuracao atual do timer
  uint8_t controlWord;
  if (timer_get_conf(timer, &controlWord)) return 1;

  // Novo comando de configuração, ativamos os bits da zona 'LSB followed by MSB' e mantemos os restantes
  controlWord = (controlWord & 0x0F) | TIMER_LSB_MSB; 

  //calcular o valor inicial do contador
  uint32_t initialValue = TIMER_FREQ / freq;
  uint8_t MSB, LSB;
  if (util_get_MSB(initialValue, &MSB)) return 1;
  if (util_get_LSB(initialValue, &LSB)) return 1;

  //atualizar a controlWord de acordo com o timer escolhido
  //selectedTimer possui a porta para o timer escolhido (40h,41h,42h)
  // "|=" -> OU BIT a BIT
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

  // Injetar o valor inicial do contador (LSB followed by MSB) diretamente no registo correspondente
  if (sys_outb(selectedTimer, LSB)) return 1;
  if (sys_outb(selectedTimer, MSB)) return 1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if( bit_no == NULL) return 1;   // validar o apontador
  

  *bit_no = BIT(hook_id);
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0) return 1;

  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != 0) return 1;
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

//ler o status do timer, nao o counter
//st -> status do timer
int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL || timer > 2 || timer < 0) return 1; //validar o timer e o st

  //RBC -> read-back command, usado para obter informações sobre o estado atual do timer
  //ativar o RBC, desativar o count (logica negada), selectionar o timer
  // bit(7) | bit (6) | bit(5) | t
  uint8_t RBC = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  
  //antes de ler é necessario enviar o READ-BACK CMD para avisar o timer a preparar a informação (pedido de GET)
  if (sys_outb(TIMER_CTRL, RBC)) return 1;

  //ler o status do timer
  if (util_sys_inb(0x40 + timer, st)) return 1;

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  if (timer > 2 || timer < 0) return 1;   //validar o timer
  
  //configuracao que nao sabemos
  union timer_status_field_val conf;

  //descubrir o tipo do field (enum)
  switch (field) {

    //tsf_all-> Display status byte, in hexadecimal
    case tsf_all:
    //byte-> The status byte
      conf.byte = st;
      break;

    //Display the initialization mode, only
    case tsf_initial:
      //BIT (4) e BIT (5)
      st = (st >> 4) & 0x03;

      switch (st) {
        //01-> LSB
        case (1):
          conf.in_mode = LSB_only;
          break;

        //10-> MSB
        case (2):
          conf.in_mode = MSB_only;
          break;

        //11-> LSB followed by MSB
        case (3):
          conf.in_mode = MSB_after_LSB;
          break;

        //invalid value
        default:
          conf.in_mode = INVAL_val;
          break;
      }
      break;

    //Display the counting mode, only
    case tsf_mode:

      //count_mode-> The counting mode: 0, 1,.., 5 (BIT 1,2,3)
      //em LCOM apenas até ao mode 3 (0111 = 0x7)
      conf.count_mode = (st >> 1) & 0x07;
      break;

    //Display the counting base, only 
    case tsf_base:
      //bcd-> The counting base, true if BCD
      //first bit of st
      conf.bcd = st & 0x01;
      break;

    //invalid field
    default:
      return 1;
  }

  //print config, functions defined in <lcf.h>
  if (timer_print_config(timer, field, conf)) return 1;
  
  return 0;
}
