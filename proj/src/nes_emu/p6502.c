#include "p6502.h"

#include "bus.h"

/*=================================================================
============================ 6502 CPU =============================
===================================================================
==Emulator for the 6502 CPU used in the NES and other systems. 	 ==
==DOCS: https://www.nesdev.org/obelisk-6502-guide/reference.html ==
==      https://www.masswerk.at/6502/6502_instruction_set.html	 ==
== ../doc/rockwell_r65c00_microprocessors.pdf                    ==
==                                                               ==
== Tiago Aleixo, 2024                                            ==
=================================================================*/

uint8_t fetch();
uint8_t fetched = 0x00;
uint16_t address_abs = 0x0000;
uint16_t address_rel = 0x0000;
uint8_t opcode = 0x00;
uint8_t cycles_left = 0;


//registers:
uint8_t accumulator = 0x00;
uint8_t x_reg = 0x00; 
uint8_t y_reg = 0x00; 
uint8_t stack_ptr = 0x00;
uint8_t status = 0x00; 
uint16_t program_counter = 0x0000; 
uint8_t get_flag(p6502_flag flag);
void set_flag(p6502_flag flag, bool enable);

instructionSet lookup[] = 
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

bool isCPU_complete(){
	return cycles_left == 0;
}
int instcounter = 0;
void cpu_clock(){
	if (cycles_left == 0){
		instcounter++;
		opcode = sysBus_read(program_counter);
		program_counter++;

		cycles_left = lookup[opcode].cycles;

		if (instcounter>16340)
		{
			//printf("arrived\n");
		}
		

		//printf("pc: %04x op name: %s\n",program_counter, lookup[opcode].name);
		//printf("stack_ptr: %04x\n", stack_ptr);
		uint8_t additional_cycles1 = lookup[opcode].ADR_MODE();
		uint8_t additional_cycles2 = lookup[opcode].INST_CODE();

		cycles_left += (additional_cycles1 & additional_cycles2); //if we pass page boundary we need to use an aditional clock cycle in certain instrctions

	}

	cycles_left--;
}

void cpu_init(){
	program_counter = 0x0000;
}

void cpu_reset(){
	printf("CPU RESET\n");
	//reset registers
	accumulator = 0;
	x_reg = 0;
	y_reg = 0;
	stack_ptr = 0xFD;
	status = 0x00 | unused_bit;
	address_abs = 0xFFFC; //as per docs when cpu resets it looks here for new program conter val
	uint8_t lsb = sysBus_read(address_abs);
	uint8_t msb = sysBus_read(address_abs + 1);

	program_counter = (msb << 8) | lsb;

	//reset internal vars
	address_abs = 0;
	address_rel = 0;
	fetched = 0;

	cycles_left = 8;
}

void cpu_irq(){ //interrupt request (IRQ) - can be ignored
	if(get_flag(interrupt_disable_bit) == 0){//do nothing if bit is set as it means we are not accepting interrupts
		//save current status to the stack
		sysBus_write(stack_ofset + stack_ptr, (program_counter >> 8) & 0x00FF); //save pc msb
		stack_ptr--;
		sysBus_write(stack_ofset + stack_ptr, program_counter & 0x00FF); //save pc lsb
		stack_ptr--;

		set_flag(break_bit, 0);
		set_flag(unused_bit, 1);
		set_flag(interrupt_disable_bit, 1);
		sysBus_write(stack_ofset + stack_ptr, status);
		stack_ptr--;

		//handle it
		address_abs = 0xFFFE;
		uint8_t lsb = sysBus_read(address_abs);
		uint8_t msb = sysBus_read(address_abs + 1);
		program_counter = (msb << 8) | lsb;

		cycles_left = 7;
	}
}

void cpu_nmi(){ //non maskable interrupt (NMI) - we cant ignore this one

		//save current status to the stack
		sysBus_write(stack_ofset + stack_ptr, (program_counter >> 8) & 0x00FF); //save pc msb
		stack_ptr--;
		sysBus_write(stack_ofset + stack_ptr, program_counter & 0x00FF); //save pc lsb
		stack_ptr--;

		set_flag(break_bit, 0);
		set_flag(unused_bit, 1);
		set_flag(interrupt_disable_bit, 1);
		sysBus_write(stack_ofset + stack_ptr, status);
		stack_ptr--;

		//handle it
		address_abs = 0xFFFA;
		uint8_t lsb = sysBus_read(address_abs);
		uint8_t msb = sysBus_read(address_abs + 1);
		program_counter = (msb << 8) | lsb;

		cycles_left = 8;
}


uint8_t get_flag(p6502_flag flag){
  return status & flag;
}

void set_flag(p6502_flag flag, bool enable){
  if (enable){
    status = status | flag;
  }else{
    status = status & ~flag;
  }
}

//helper:
uint8_t fetch(){
	if(!(lookup[opcode].ADR_MODE == &ADR_IMP))
		fetched = sysBus_read(address_abs);
	return fetched;
}

/*addressing modes: DOCS: https://www.nesdev.org/obelisk-6502-guide/addressing.html
                          https://www.masswerk.at/6502/6502_instruction_set.html
                          ../doc/rockwell_r65c00_microprocessors.pdf
*/

uint8_t ADR_IMP(){
	fetched = accumulator;
	return 0;
}

uint8_t ADR_IMM(){
	address_abs = program_counter++;
	return 0;
}

uint8_t ADR_ZP0(){
	address_abs = sysBus_read(program_counter++);
	address_abs = address_abs & 0x00FF; // we are reading from page 0 so we dont need MSB
	return 0;
}

uint8_t ADR_ZPX(){
	address_abs = sysBus_read(program_counter++) + x_reg;
	address_abs = address_abs & 0x00FF; // we are reading from page 0 so we dont need MSB
	return 0;
}

uint8_t ADR_ZPY(){
	address_abs = sysBus_read(program_counter) + y_reg;
	program_counter++;
	address_abs = address_abs & 0x00FF; // we are reading from page 0 so we dont need MSB
	return 0;
}

uint8_t ADR_ABS(){
	uint8_t lsb = sysBus_read(program_counter++);
	uint8_t msb = sysBus_read(program_counter++);

	address_abs = (msb << 8) | lsb;
	return 0;
}

uint8_t ADR_ABX(){
	uint8_t lsb = sysBus_read(program_counter++);
	uint8_t msb = sysBus_read(program_counter++);

	address_abs = (msb << 8) | lsb;
	address_abs += x_reg;

	if ((address_abs & 0xFF00) != (msb << 8)) {
        return 1;
    }

    return 0;
}

uint8_t ADR_ABY(){
	uint8_t lsb = sysBus_read(program_counter++);
	uint8_t msb = sysBus_read(program_counter++);

	address_abs = ((msb << 8) | lsb) + y_reg;

	if ((address_abs & 0xFF00) != (msb << 8)) return 1; //if this makes us read an address on a different page we may need another clock cycle
	return 0;
}


uint8_t ADR_IND(){
	uint8_t lsb = sysBus_read(program_counter++);
	uint8_t msb = sysBus_read(program_counter++);

	uint16_t ptr = (msb << 8) | lsb;

	address_abs = (sysBus_read(program_counter + 1) << 8) | sysBus_read(program_counter);
	
    if (ptr == 0x00FF) {
        address_abs = (sysBus_read(ptr & 0xFF00) << 8) | sysBus_read(ptr + 0);
    }
    // Normal behaivour
    else {
        address_abs = (sysBus_read(ptr + 1) << 8) | sysBus_read(ptr + 0);
    }
	
	return 0;
}

uint8_t ADR_IZX(){
	uint8_t ptr = sysBus_read(program_counter++);
	uint8_t lsb = sysBus_read((uint16_t)(ptr + (uint16_t)x_reg) & 0x00FF);
	uint8_t msb = sysBus_read((uint16_t)(ptr + (uint16_t)x_reg + 1) & 0x00FF);
	
	address_abs = (msb << 8) | lsb;
	return 0;
}

uint8_t ADR_IZY(){
	uint8_t ptr = sysBus_read(program_counter++);
	uint8_t lsb = sysBus_read((uint16_t)(ptr + (uint16_t)y_reg) & 0x00FF);
	uint8_t msb = sysBus_read((uint16_t)(ptr + (uint16_t)y_reg + 1) & 0x00FF);
	address_abs = ((msb << 8) | lsb) + y_reg;

    // If the addr is in a new page, then we may need another clock cycle
    if ((address_abs & 0xFF00) != (msb << 8)) {
        return 1;
    }
	return 0;
}

uint8_t ADR_REL(){
	address_rel = sysBus_read(program_counter);
	program_counter++;
	
	if (address_rel & BIT(7)) address_rel |= 0xFF00;
	return 0;
}

/*Instructions  DOCS: https://www.nesdev.org/obelisk-6502-guide/reference.html
                      https://www.masswerk.at/6502/6502_instruction_set.html
                      ../doc/rockwell_r65c00_microprocessors.pdf
                      ../doc/instructionset.png


some instructions have a return value of 1, this means that they may need an extra clock cycle to execute
check here: https://www.nesdev.org/obelisk-6502-guide/reference.html to see if the instruction  needs an extra clock cycle and in what situation
*/

uint8_t INST_RTI(){ //return from interrupt
	stack_ptr++;
	status = sysBus_read(stack_ofset + stack_ptr);
	status &= ~break_bit;
	status &= ~unused_bit;

	stack_ptr++;
	program_counter = (uint16_t)sysBus_read(stack_ofset + stack_ptr);
	stack_ptr++;
	program_counter |= (uint16_t)sysBus_read(stack_ofset + stack_ptr) << 8;

	return 0;
}

uint8_t INST_AND(){
	fetch();
	accumulator &= fetched;
	set_flag(zero_bit, accumulator == 0x00);
	set_flag(negative_bit, (accumulator & BIT(7)) != 0);

	return 1; 
}

uint8_t INST_BCC(){
	if(get_flag(carry_bit) == 0){
		cycles_left++;
		address_abs = program_counter + address_rel;

		if((address_abs & 0xFF00) != (program_counter & 0xFF00)) cycles_left++;

		program_counter = address_abs;
	}
	return 0;
}

uint8_t INST_BCS(){
	if(get_flag(carry_bit) == 1){
		cycles_left++;
		address_abs = program_counter + address_rel;

		if((address_abs & 0xFF00) != (program_counter & 0xFF00)) cycles_left++;

		program_counter = address_abs;
	}
	return 0;
}

uint8_t INST_BEQ(){
	if(get_flag(zero_bit) == 1){
		cycles_left++;
		address_abs = program_counter + address_rel;

		if((address_abs & 0xFF00) != (program_counter & 0xFF00)) cycles_left++;

		program_counter = address_abs;
	}
	return 0;
}

uint8_t INST_BIT(){
	fetch();
	uint16_t temp = accumulator & fetched;
	set_flag(zero_bit, (temp & 0x00FF) == 0);
	set_flag(overflow_bit, (fetched & BIT(6)) != 0);
	set_flag(negative_bit, (fetched & BIT(7)) != 0);
	return 0;
}

uint8_t INST_BMI(){
	if(get_flag(negative_bit) == 1){
		cycles_left++;
		address_abs = program_counter + address_rel;

		if((address_abs & 0xFF00) != (program_counter & 0xFF00)) cycles_left++;

		program_counter = address_abs;
	}
	return 0;
}

uint8_t INST_BNE(){
	if(get_flag(zero_bit) == 0){
		cycles_left++;
		address_abs = program_counter + address_rel;

		if((address_abs & 0xFF00) != (program_counter & 0xFF00)) cycles_left++;

		program_counter = address_abs;
	}
	return 0;
}

uint8_t INST_BPL(){
	if(get_flag(negative_bit) == 0){
		cycles_left++;
		address_abs = program_counter + address_rel;

		if((address_abs & 0xFF00) != (program_counter & 0xFF00)) cycles_left++;

		program_counter = address_abs;
	}
	return 0;
}

uint8_t INST_BRK(){
	program_counter++;
	set_flag(interrupt_disable_bit, true);
	sysBus_write(stack_ofset + stack_ptr, (program_counter >> 8) & 0x00FF); //save pc msb
	stack_ptr--;
	sysBus_write(stack_ofset + stack_ptr, program_counter & 0x00FF); //save pc lsb
	stack_ptr--;

	set_flag(break_bit, true);
	sysBus_write(stack_ofset + stack_ptr, status);
	stack_ptr--;
	set_flag(break_bit, false);

	program_counter = (uint16_t)sysBus_read(0xFFFE) | ((uint16_t)sysBus_read(0xFFFF) << 8);

	return 0;
}

uint8_t INST_BVC(){
	if(get_flag(overflow_bit) == 0){
		cycles_left++;
		address_abs = program_counter + address_rel;

		if((address_abs & 0xFF00) != (program_counter & 0xFF00)) cycles_left++;

		program_counter = address_abs;
	}
	return 0;
}

uint8_t INST_BVS(){
	if(get_flag(overflow_bit) == 1){
		cycles_left++;
		address_abs = program_counter + address_rel;

		if((address_abs & 0xFF00) != (program_counter & 0xFF00)) cycles_left++;

		program_counter = address_abs;
	}
	return 0;
}


uint8_t INST_ASL(){
	fetch();
	uint16_t temp = (uint16_t)fetched << 1;
	set_flag(carry_bit, (temp & 0xFF00) > 0);
	set_flag(zero_bit, (temp & 0x00FF) == 0);
	set_flag(negative_bit, temp & BIT(7));

	if(lookup[opcode].ADR_MODE == &ADR_IMP){
		accumulator = temp & 0x00FF;
	}else{
		sysBus_write(address_abs, temp & 0x00FF);
	}
	return 0;
}

uint8_t INST_CLC(){
	set_flag(carry_bit, false);
	return 0;
}

uint8_t INST_CLD(){
	set_flag(decimal_mode_bit, false);
	return 0;
}

uint8_t INST_CLI(){
	set_flag(interrupt_disable_bit, false);
	return 0;
}

uint8_t INST_CLV() {
	set_flag(overflow_bit, false);
  return 0;
}

uint8_t INST_CMP(){
	fetch();
	uint16_t val = (uint16_t)accumulator - (uint16_t)fetched;
	set_flag(carry_bit, accumulator >= fetched);
	set_flag(zero_bit, (val & 0x00FF) == 0);
	set_flag(negative_bit, val & BIT(7));
	return 1;
}

uint8_t INST_CPX(){
	fetch();
	uint16_t val = (uint16_t)x_reg - (uint16_t)fetched;
	set_flag(carry_bit, x_reg >= fetched);
	set_flag(zero_bit, (val & 0x00FF) == 0);
	set_flag(negative_bit, val & BIT(7));
	return 0;
}

uint8_t INST_CPY(){
	fetch();
	uint16_t val = (uint16_t)y_reg - (uint16_t)fetched;
	set_flag(carry_bit, y_reg >= fetched);
	set_flag(zero_bit, (val & 0x00FF) == 0);
	set_flag(negative_bit, val & BIT(7));
	return 0;
}


uint8_t INST_DEC(){
	fetch();
	uint8_t temp = fetched - 1;
	sysBus_write(address_abs, temp);
	set_flag(zero_bit, temp == 0);
	set_flag(negative_bit, temp & BIT(7));
	return 0;
}

uint8_t INST_DEX(){
	x_reg--;
	set_flag(zero_bit, x_reg == 0);
	set_flag(negative_bit, x_reg & BIT(7));
	return 0;
}

uint8_t INST_DEY(){
	y_reg--;
	set_flag(zero_bit, y_reg == 0);
	set_flag(negative_bit, y_reg & BIT(7));
	return 0;
}

uint8_t INST_EOR(){
	fetch();
	accumulator ^= fetched;
	set_flag(zero_bit, accumulator == 0x00);
	set_flag(negative_bit, (accumulator & BIT(7)) != 0);
	return 1;
}

uint8_t INST_INC(){
	fetch();
	uint8_t temp = fetched + 1;
	sysBus_write(address_abs, temp);
	set_flag(zero_bit, temp == 0);
	set_flag(negative_bit, temp & BIT(7));
	return 0;
}

uint8_t INST_INX(){
	x_reg++;
	set_flag(zero_bit, x_reg == 0);
	set_flag(negative_bit, x_reg & BIT(7));
	return 0;
}

uint8_t INST_INY(){
	y_reg++;
	set_flag(zero_bit, y_reg == 0);
	set_flag(negative_bit, y_reg & BIT(7));
	return 0;
}

uint8_t INST_JMP(){ //todo recreate bug
	program_counter = address_abs;
	return 0;
}

uint8_t INST_JSR(){
	program_counter--;

	sysBus_write(stack_ofset + stack_ptr, (program_counter >> 8) & 0x00FF);
	stack_ptr--;
	sysBus_write(stack_ofset + stack_ptr, program_counter & 0x00FF);
	stack_ptr--;

	program_counter = address_abs;
	return 0;
}

uint8_t INST_LDA(){
	fetch();
	accumulator = fetched;
	set_flag(zero_bit, accumulator == 0x00);
	set_flag(negative_bit, (accumulator & BIT(7)) != 0);
	return 1;
}

uint8_t INST_LDX(){
	fetch();
	x_reg = fetched;
	set_flag(zero_bit, x_reg == 0x00);
	set_flag(negative_bit, (x_reg & BIT(7)) != 0);
	return 1;
}

uint8_t INST_LDY(){
	fetch();
	y_reg = fetched;
	set_flag(zero_bit, y_reg == 0x00);
	set_flag(negative_bit, (y_reg & BIT(7)) != 0);
	return 1;
}

uint8_t INST_LSR(){
	fetch();
	set_flag(carry_bit, fetched & 0x0001);
	uint16_t temp = fetched >> 1;
	set_flag(zero_bit, (temp & 0x00FF) == 0);
	set_flag(negative_bit, temp & BIT(7));

	if(lookup[opcode].ADR_MODE == &ADR_IMP){
		accumulator = temp & 0x00FF;
	}else{
		sysBus_write(address_abs, temp & 0x00FF);
	}
	return 0;
}

uint8_t INST_NOP(){ //todo add more illegal opcodes
	switch(opcode){ 
		case 0x1C:
		case 0x3C:
		case 0x5C:
		case 0x7C:
		case 0xDC:
		case 0xFC:
			return 1;
	}
	return 0;
}

uint8_t INST_ORA(){
	fetch();
	accumulator |= fetched;
	set_flag(zero_bit, accumulator == 0x00);
	set_flag(negative_bit, (accumulator & BIT(7)) != 0);
	return 1;
}



uint8_t INST_ADC(){
	fetch();
	uint16_t sum = (uint16_t)accumulator + (uint16_t)fetched + (uint16_t)get_flag(carry_bit);
	set_flag(carry_bit, sum > 255);
	set_flag(zero_bit, (sum & 0x00FF) == 0);
	set_flag(negative_bit, sum & BIT(7));

	set_flag(overflow_bit, (~((uint16_t)accumulator ^ (uint16_t)fetched) & ((uint16_t)accumulator ^ (uint16_t)sum)) & 0x0080);

	accumulator = sum & 0x00FF;
	return 1;
}

uint8_t INST_SUB(){
	fetch();
	uint16_t val = ((uint16_t)fetched ^ 0x00FF);
	uint16_t sub = (uint16_t)accumulator + val + (uint16_t)get_flag(carry_bit);
	set_flag(carry_bit, sub > 255);
	set_flag(zero_bit, (sub & 0x00FF) == 0);
	set_flag(negative_bit, sub & BIT(7));

	set_flag(overflow_bit, (~((uint16_t)accumulator ^ val) & ((uint16_t)accumulator ^ (uint16_t)sub)) & 0x0080);

	accumulator = sub & 0x00FF;
	return 1;
}

uint8_t INST_PHA(){
	sysBus_write(stack_ofset + stack_ptr, accumulator);
	stack_ptr--;
	return 0;
}

uint8_t INST_PHP(){
	sysBus_write(stack_ofset + stack_ptr, status | break_bit | unused_bit);
	set_flag(break_bit, 0);
	set_flag(unused_bit, 0);
	stack_ptr--;
	return 0;
}

uint8_t INST_PLA(){
	stack_ptr++;
	accumulator = sysBus_read(stack_ofset + stack_ptr);
	set_flag(zero_bit, accumulator == 0);
	set_flag(negative_bit, accumulator & BIT(7));
	return 0;
}

uint8_t INST_PLP(){
	stack_ptr++;
	status = sysBus_read(stack_ofset + stack_ptr);
	set_flag(unused_bit, 1);
	return 0;
}

uint8_t INST_ROL(){
	fetch();
	uint16_t temp = (uint16_t)(fetched << 1) | get_flag(carry_bit);
	set_flag(carry_bit, (temp & 0xFF00) > 0);
	set_flag(zero_bit, (temp & 0x00FF) == 0);
	set_flag(negative_bit, temp & BIT(7));

	if(lookup[opcode].ADR_MODE == &ADR_IMP){
		accumulator = temp & 0x00FF;
	}else{
		sysBus_write(address_abs, temp & 0x00FF);
	}
	return 0;
}

uint8_t INST_ROR(){
	fetch();
	uint16_t temp = (uint16_t)(get_flag(carry_bit) << 7) | (fetched >> 1);
	set_flag(carry_bit, fetched & 0x0001);
	set_flag(zero_bit, (temp & 0x00FF) == 0);
	set_flag(negative_bit, temp & BIT(7));

	if(lookup[opcode].ADR_MODE == &ADR_IMP){
		accumulator = temp & 0x00FF;
	}else{
		sysBus_write(address_abs, temp & 0x00FF);
	}
	return 0;
}

uint8_t INST_RTS(){
	stack_ptr++;
	program_counter = sysBus_read(stack_ofset + stack_ptr);
	stack_ptr++;
	program_counter |= (uint16_t)sysBus_read(stack_ofset + stack_ptr) << 8;

	program_counter++;
	return 0;
}

uint8_t INST_SBC(){
	fetch();
	uint16_t val = ((uint16_t)fetched) ^ 0x00FF;
	uint16_t sub = (uint16_t)accumulator + val + (uint16_t)get_flag(carry_bit);
	set_flag(carry_bit, sub > 255);
	set_flag(zero_bit, (sub & 0x00FF) == 0);
	set_flag(negative_bit, sub & BIT(7));

	set_flag(overflow_bit, (~((uint16_t)accumulator ^ val) & ((uint16_t)accumulator ^ (uint16_t)sub)) & 0x0080);

	accumulator = sub & 0x00FF;
	return 1;
}

uint8_t INST_SEC(){
	set_flag(carry_bit, true);
	return 0;
}

uint8_t INST_SED(){
	set_flag(decimal_mode_bit, true);
	return 0;
}

uint8_t INST_SEI(){
	set_flag(interrupt_disable_bit, true);
	return 0;
}

uint8_t INST_STA(){
	sysBus_write(address_abs, accumulator);
	return 0;
}	

uint8_t INST_STX(){
	sysBus_write(address_abs, x_reg);
	return 0;
}

uint8_t INST_STY(){
	sysBus_write(address_abs, y_reg);
	return 0;
}

uint8_t INST_TAX(){
	x_reg = accumulator;
	set_flag(zero_bit, x_reg == 0);
	set_flag(negative_bit, x_reg & BIT(7));
	return 0;
}

uint8_t INST_TAY(){
	y_reg = accumulator;
	set_flag(zero_bit, y_reg == 0);
	set_flag(negative_bit, y_reg & BIT(7));
	return 0;
}

uint8_t INST_TSX(){
	x_reg = stack_ptr;
	set_flag(zero_bit, x_reg == 0);
	set_flag(negative_bit, x_reg & BIT(7));
	return 0;
}

uint8_t INST_TXA(){
	accumulator = x_reg;
	set_flag(zero_bit, accumulator == 0);
	set_flag(negative_bit, accumulator & BIT(7));
	return 0;
}

uint8_t INST_TXS(){
	stack_ptr = x_reg;
	return 0;
}

uint8_t INST_TYA(){
	accumulator = y_reg;
	set_flag(zero_bit, accumulator == 0);
	set_flag(negative_bit, accumulator & BIT(7));
	return 0;
}

uint8_t INST_XXX(){
	return 0;
}

