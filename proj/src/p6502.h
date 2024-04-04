#include <lcom/lcf.h>
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
void cpu_reset();
void cpu_irq(); //these can be ignore depending on policy
void cpu_mni(); //these cannot be ignored!

static uint8_t fetch();
static uint8_t fetched = 0x00;

static uint16_t address_abs = 0x0000;
static uint16_t address_rel = 0x0000;
static uint8_t opcode = 0x00;
static uint8_t cycles_left = 0;


//registers:
static uint8_t accumulator = 0x00;
static uint8_t x_reg = 0x00; 
static uint8_t y_reg = 0x00; 
static uint8_t stack_ptr = 0x00;
static uint8_t status = 0x00; 
static uint16_t program_counter = 0x0000; 

static uint8_t get_flag(p6502_flag flag);
static void set_flag(p6502_flag flag, bool enable);


//addresing modes:

static uint8_t ADR_IMP(); static uint8_t ADR_IMM();
static uint8_t ADR_ZP0(); static uint8_t ADR_ZPX();
static uint8_t ADR_ZPY(); static uint8_t ADR_REL();
static uint8_t ADR_ABS(); static uint8_t ADR_ABX();
static uint8_t ADR_ABY(); static uint8_t ADR_IND();
static uint8_t ADR_IZX(); static uint8_t ADR_IZY();


//opcodes:
	static uint8_t INST_ADC();	static uint8_t INST_AND();	static uint8_t INST_ASL(); static uint8_t INST_BCC();
	static uint8_t INST_BCS();	static uint8_t INST_BEQ();	static uint8_t INST_BIT(); static uint8_t INST_BMI();
	static uint8_t INST_BNE();	static uint8_t INST_BPL();	static uint8_t INST_BRK(); static uint8_t INST_BVC();
	static uint8_t INST_BVS();	static uint8_t INST_CLC();	static uint8_t INST_CLD(); static uint8_t INST_CLI();
	static uint8_t INST_CLV();	static uint8_t INST_CMP();	static uint8_t INST_CPX(); static uint8_t INST_CPY();
	static uint8_t INST_DEC();	static uint8_t INST_DEX();	static uint8_t INST_DEY(); static uint8_t INST_EOR();
	static uint8_t INST_INC();	static uint8_t INST_INX();	static uint8_t INST_INY(); static uint8_t INST_JMP();
	static uint8_t INST_JSR();	static uint8_t INST_LDA();	static uint8_t INST_LDX(); static uint8_t INST_LDY();
	static uint8_t INST_LSR();	static uint8_t INST_NOP();	static uint8_t INST_ORA(); static uint8_t INST_PHA();
	static uint8_t INST_PHP();	static uint8_t INST_PLA();	static uint8_t INST_PLP(); static uint8_t INST_ROL();
	static uint8_t INST_ROR();	static uint8_t INST_RTI();	static uint8_t INST_RTS(); static uint8_t INST_SBC();
	static uint8_t INST_SEC();	static uint8_t INST_SED();	static uint8_t INST_SEI(); static uint8_t INST_STA();
	static uint8_t INST_STX();	static uint8_t INST_STY();	static uint8_t INST_TAX(); static uint8_t INST_TAY();
	static uint8_t INST_TSX();	static uint8_t INST_TXA();	static uint8_t INST_TXS(); static uint8_t INST_TYA();

  static uint8_t INST_XXX(); //if opcode dosent exist

  typedef struct{
    char name[3]; //name
    int (*INST_CODE)();
    int (*ADR_MODE)();
    uint8_t cycles;
  }instructionSet;

  static instructionSet lookup[] = 
    {
      { "BRK", &INST_BRK, &ADR_IMM, 7 },{ "ORA", &INST_ORA, &ADR_IZX, 6 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 3 },{ "ORA", &INST_ORA, &ADR_ZP0, 3 },{ "ASL", &INST_ASL, &ADR_ZP0, 5 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "PHP", &INST_PHP, &ADR_IMP, 3 },{ "ORA", &INST_ORA, &ADR_IMM, 2 },{ "ASL", &INST_ASL, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "ORA", &INST_ORA, &ADR_ABS, 4 },{ "ASL", &INST_ASL, &ADR_ABS, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },
      { "BPL", &INST_BPL, &ADR_REL, 2 },{ "ORA", &INST_ORA, &ADR_IZY, 5 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "ORA", &INST_ORA, &ADR_ZPX, 4 },{ "ASL", &INST_ASL, &ADR_ZPX, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "CLC", &INST_CLC, &ADR_IMP, 2 },{ "ORA", &INST_ORA, &ADR_ABY, 4 },{ "???", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 7 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "ORA", &INST_ORA, &ADR_ABX, 4 },{ "ASL", &INST_ASL, &ADR_ABX, 7 },{ "???", &INST_XXX, &ADR_IMP, 7 },
      { "JSR", &INST_JSR, &ADR_ABS, 6 },{ "AND", &INST_AND, &ADR_IZX, 6 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "BIT", &INST_BIT, &ADR_ZP0, 3 },{ "AND", &INST_AND, &ADR_ZP0, 3 },{ "ROL", &INST_ROL, &ADR_ZP0, 5 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "PLP", &INST_PLP, &ADR_IMP, 4 },{ "AND", &INST_AND, &ADR_IMM, 2 },{ "ROL", &INST_ROL, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "BIT", &INST_BIT, &ADR_ABS, 4 },{ "AND", &INST_AND, &ADR_ABS, 4 },{ "ROL", &INST_ROL, &ADR_ABS, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },
      { "BMI", &INST_BMI, &ADR_REL, 2 },{ "AND", &INST_AND, &ADR_IZY, 5 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "AND", &INST_AND, &ADR_ZPX, 4 },{ "ROL", &INST_ROL, &ADR_ZPX, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "SEC", &INST_SEC, &ADR_IMP, 2 },{ "AND", &INST_AND, &ADR_ABY, 4 },{ "???", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 7 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "AND", &INST_AND, &ADR_ABX, 4 },{ "ROL", &INST_ROL, &ADR_ABX, 7 },{ "???", &INST_XXX, &ADR_IMP, 7 },
      { "RTI", &INST_RTI, &ADR_IMP, 6 },{ "EOR", &INST_EOR, &ADR_IZX, 6 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 3 },{ "EOR", &INST_EOR, &ADR_ZP0, 3 },{ "LSR", &INST_LSR, &ADR_ZP0, 5 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "PHA", &INST_PHA, &ADR_IMP, 3 },{ "EOR", &INST_EOR, &ADR_IMM, 2 },{ "LSR", &INST_LSR, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "JMP", &INST_JMP, &ADR_ABS, 3 },{ "EOR", &INST_EOR, &ADR_ABS, 4 },{ "LSR", &INST_LSR, &ADR_ABS, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },
      { "BVC", &INST_BVC, &ADR_REL, 2 },{ "EOR", &INST_EOR, &ADR_IZY, 5 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "EOR", &INST_EOR, &ADR_ZPX, 4 },{ "LSR", &INST_LSR, &ADR_ZPX, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "CLI", &INST_CLI, &ADR_IMP, 2 },{ "EOR", &INST_EOR, &ADR_ABY, 4 },{ "???", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 7 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "EOR", &INST_EOR, &ADR_ABX, 4 },{ "LSR", &INST_LSR, &ADR_ABX, 7 },{ "???", &INST_XXX, &ADR_IMP, 7 },
      { "RTS", &INST_RTS, &ADR_IMP, 6 },{ "ADC", &INST_ADC, &ADR_IZX, 6 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 3 },{ "ADC", &INST_ADC, &ADR_ZP0, 3 },{ "ROR", &INST_ROR, &ADR_ZP0, 5 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "PLA", &INST_PLA, &ADR_IMP, 4 },{ "ADC", &INST_ADC, &ADR_IMM, 2 },{ "ROR", &INST_ROR, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "JMP", &INST_JMP, &ADR_IND, 5 },{ "ADC", &INST_ADC, &ADR_ABS, 4 },{ "ROR", &INST_ROR, &ADR_ABS, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },
      { "BVS", &INST_BVS, &ADR_REL, 2 },{ "ADC", &INST_ADC, &ADR_IZY, 5 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "ADC", &INST_ADC, &ADR_ZPX, 4 },{ "ROR", &INST_ROR, &ADR_ZPX, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "SEI", &INST_SEI, &ADR_IMP, 2 },{ "ADC", &INST_ADC, &ADR_ABY, 4 },{ "???", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 7 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "ADC", &INST_ADC, &ADR_ABX, 4 },{ "ROR", &INST_ROR, &ADR_ABX, 7 },{ "???", &INST_XXX, &ADR_IMP, 7 },
      { "???", &INST_NOP, &ADR_IMP, 2 },{ "STA", &INST_STA, &ADR_IZX, 6 },{ "???", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "STY", &INST_STY, &ADR_ZP0, 3 },{ "STA", &INST_STA, &ADR_ZP0, 3 },{ "STX", &INST_STX, &ADR_ZP0, 3 },{ "???", &INST_XXX, &ADR_IMP, 3 },{ "DEY", &INST_DEY, &ADR_IMP, 2 },{ "???", &INST_NOP, &ADR_IMP, 2 },{ "TXA", &INST_TXA, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "STY", &INST_STY, &ADR_ABS, 4 },{ "STA", &INST_STA, &ADR_ABS, 4 },{ "STX", &INST_STX, &ADR_ABS, 4 },{ "???", &INST_XXX, &ADR_IMP, 4 },
      { "BCC", &INST_BCC, &ADR_REL, 2 },{ "STA", &INST_STA, &ADR_IZY, 6 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "STY", &INST_STY, &ADR_ZPX, 4 },{ "STA", &INST_STA, &ADR_ZPX, 4 },{ "STX", &INST_STX, &ADR_ZPY, 4 },{ "???", &INST_XXX, &ADR_IMP, 4 },{ "TYA", &INST_TYA, &ADR_IMP, 2 },{ "STA", &INST_STA, &ADR_ABY, 5 },{ "TXS", &INST_TXS, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "???", &INST_NOP, &ADR_IMP, 5 },{ "STA", &INST_STA, &ADR_ABX, 5 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "???", &INST_XXX, &ADR_IMP, 5 },
      { "LDY", &INST_LDY, &ADR_IMM, 2 },{ "LDA", &INST_LDA, &ADR_IZX, 6 },{ "LDX", &INST_LDX, &ADR_IMM, 2 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "LDY", &INST_LDY, &ADR_ZP0, 3 },{ "LDA", &INST_LDA, &ADR_ZP0, 3 },{ "LDX", &INST_LDX, &ADR_ZP0, 3 },{ "???", &INST_XXX, &ADR_IMP, 3 },{ "TAY", &INST_TAY, &ADR_IMP, 2 },{ "LDA", &INST_LDA, &ADR_IMM, 2 },{ "TAX", &INST_TAX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "LDY", &INST_LDY, &ADR_ABS, 4 },{ "LDA", &INST_LDA, &ADR_ABS, 4 },{ "LDX", &INST_LDX, &ADR_ABS, 4 },{ "???", &INST_XXX, &ADR_IMP, 4 },
      { "BCS", &INST_BCS, &ADR_REL, 2 },{ "LDA", &INST_LDA, &ADR_IZY, 5 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "LDY", &INST_LDY, &ADR_ZPX, 4 },{ "LDA", &INST_LDA, &ADR_ZPX, 4 },{ "LDX", &INST_LDX, &ADR_ZPY, 4 },{ "???", &INST_XXX, &ADR_IMP, 4 },{ "CLV", &INST_CLV, &ADR_IMP, 2 },{ "LDA", &INST_LDA, &ADR_ABY, 4 },{ "TSX", &INST_TSX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 4 },{ "LDY", &INST_LDY, &ADR_ABX, 4 },{ "LDA", &INST_LDA, &ADR_ABX, 4 },{ "LDX", &INST_LDX, &ADR_ABY, 4 },{ "???", &INST_XXX, &ADR_IMP, 4 },
      { "CPY", &INST_CPY, &ADR_IMM, 2 },{ "CMP", &INST_CMP, &ADR_IZX, 6 },{ "???", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "CPY", &INST_CPY, &ADR_ZP0, 3 },{ "CMP", &INST_CMP, &ADR_ZP0, 3 },{ "DEC", &INST_DEC, &ADR_ZP0, 5 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "INY", &INST_INY, &ADR_IMP, 2 },{ "CMP", &INST_CMP, &ADR_IMM, 2 },{ "DEX", &INST_DEX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "CPY", &INST_CPY, &ADR_ABS, 4 },{ "CMP", &INST_CMP, &ADR_ABS, 4 },{ "DEC", &INST_DEC, &ADR_ABS, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },
      { "BNE", &INST_BNE, &ADR_REL, 2 },{ "CMP", &INST_CMP, &ADR_IZY, 5 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "CMP", &INST_CMP, &ADR_ZPX, 4 },{ "DEC", &INST_DEC, &ADR_ZPX, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "CLD", &INST_CLD, &ADR_IMP, 2 },{ "CMP", &INST_CMP, &ADR_ABY, 4 },{ "NOP", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 7 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "CMP", &INST_CMP, &ADR_ABX, 4 },{ "DEC", &INST_DEC, &ADR_ABX, 7 },{ "???", &INST_XXX, &ADR_IMP, 7 },
      { "CPX", &INST_CPX, &ADR_IMM, 2 },{ "SBC", &INST_SBC, &ADR_IZX, 6 },{ "???", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "CPX", &INST_CPX, &ADR_ZP0, 3 },{ "SBC", &INST_SBC, &ADR_ZP0, 3 },{ "INC", &INST_INC, &ADR_ZP0, 5 },{ "???", &INST_XXX, &ADR_IMP, 5 },{ "INX", &INST_INX, &ADR_IMP, 2 },{ "SBC", &INST_SBC, &ADR_IMM, 2 },{ "NOP", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_SBC, &ADR_IMP, 2 },{ "CPX", &INST_CPX, &ADR_ABS, 4 },{ "SBC", &INST_SBC, &ADR_ABS, 4 },{ "INC", &INST_INC, &ADR_ABS, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },
      { "BEQ", &INST_BEQ, &ADR_REL, 2 },{ "SBC", &INST_SBC, &ADR_IZY, 5 },{ "???", &INST_XXX, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 8 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "SBC", &INST_SBC, &ADR_ZPX, 4 },{ "INC", &INST_INC, &ADR_ZPX, 6 },{ "???", &INST_XXX, &ADR_IMP, 6 },{ "SED", &INST_SED, &ADR_IMP, 2 },{ "SBC", &INST_SBC, &ADR_ABY, 4 },{ "NOP", &INST_NOP, &ADR_IMP, 2 },{ "???", &INST_XXX, &ADR_IMP, 7 },{ "???", &INST_NOP, &ADR_IMP, 4 },{ "SBC", &INST_SBC, &ADR_ABX, 4 },{ "INC", &INST_INC, &ADR_ABX, 7 },{ "???", &INST_XXX, &ADR_IMP, 7 },
    };
    