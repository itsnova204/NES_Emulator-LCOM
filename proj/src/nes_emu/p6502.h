#include "lcf_mock.h"

#pragma once

typedef enum{
  carry_bit = BIT(0),
  zero_bit = BIT(1),
  interrupt_disable_bit = BIT(2),
  decimal_mode_bit = BIT(3), //not used as the 6502 version used in NES didnt have this
  break_bit = BIT(4),
  unused_bit = BIT(5), // this is always 1
  overflow_bit = BIT(6),
  negative_bit = BIT(7),
} p6502_flag;

#define  stack_ofset 0x0100

void cpu_clock();
void cpu_init();
void cpu_reset();
void cpu_irq(); //these can be ignore depending on policy
void cpu_nmi(); //these cannot be ignored!

//addresing modes:

uint8_t ADR_IMP(); uint8_t ADR_IMM();
uint8_t ADR_ZP0(); uint8_t ADR_ZPX();
uint8_t ADR_ZPY(); uint8_t ADR_REL();
uint8_t ADR_ABS(); uint8_t ADR_ABX();
uint8_t ADR_ABY(); uint8_t ADR_IND();
uint8_t ADR_IZX(); uint8_t ADR_IZY();


//opcodes:
uint8_t INST_ADC();	uint8_t INST_AND();	uint8_t INST_ASL(); uint8_t INST_BCC();
uint8_t INST_BCS();	uint8_t INST_BEQ();	uint8_t INST_BIT(); uint8_t INST_BMI();
uint8_t INST_BNE();	uint8_t INST_BPL();	uint8_t INST_BRK(); uint8_t INST_BVC();
uint8_t INST_BVS();	uint8_t INST_CLC();	uint8_t INST_CLD(); uint8_t INST_CLI();
uint8_t INST_CLV();	uint8_t INST_CMP();	uint8_t INST_CPX(); uint8_t INST_CPY();
uint8_t INST_DEC();	uint8_t INST_DEX();	uint8_t INST_DEY(); uint8_t INST_EOR();
uint8_t INST_INC();	uint8_t INST_INX();	uint8_t INST_INY(); uint8_t INST_JMP();
uint8_t INST_JSR();	uint8_t INST_LDA();	uint8_t INST_LDX(); uint8_t INST_LDY();
uint8_t INST_LSR();	uint8_t INST_NOP();	uint8_t INST_ORA(); uint8_t INST_PHA();
uint8_t INST_PHP();	uint8_t INST_PLA();	uint8_t INST_PLP(); uint8_t INST_ROL();
uint8_t INST_ROR();	uint8_t INST_RTI();	uint8_t INST_RTS(); uint8_t INST_SBC();
uint8_t INST_SEC();	uint8_t INST_SED();	uint8_t INST_SEI(); uint8_t INST_STA();
uint8_t INST_STX();	uint8_t INST_STY();	uint8_t INST_TAX(); uint8_t INST_TAY();
uint8_t INST_TSX();	uint8_t INST_TXA();	uint8_t INST_TXS(); uint8_t INST_TYA();

uint8_t INST_XXX(); //if opcode dosent exist

  typedef struct{
    char name[4]; //name
    uint8_t (*INST_CODE)();
    uint8_t (*ADR_MODE)();
    uint8_t cycles;
  }instructionSet;


