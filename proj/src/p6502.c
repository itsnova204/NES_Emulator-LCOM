#include <lcom/lcf.h>
#include "p6502.h"
#include "bus.h"

/*=================================================================
============================ 6502 CPU =============================
===================================================================
==Emulator for the 6502 CPU used in the NES and other systems. 	 ==
==DOCS: https://www.nesdev.org/obelisk-6502-guide/reference.html ==
== https://www.masswerk.at/6502/6502_instruction_set.html	 ==
== ../doc/rockwell_r65c00_microprocessors.pdf                    ==
==                                                               ==
== Tiago Aleixo, 2024                                            ==
=================================================================*/

void cpu_clock(){
	if (cycles_left == 0){
		opcode = bus_read(program_counter);
		program_counter++;

		cycles_left = lookup[opcode].cycles;

		uint8_t additional_cycles1 = lookup[opcode].ADR_MODE;
		uint8_t additional_cycles2 = lookup[opcode].INST_CODE;

		cycles_left += (additional_cycles1 & additional_cycles2); //if we pass page boundary we need to use an aditional clock cycle in certain instrctions
	}

	cycles_left--;
}

void cpu_reset(){
	//reset registers
	accumulator = 0;
	x_reg = 0;
	y_reg = 0;
	stack_ptr = 0xFD;
	status = 0x00 | unused_bit;
	address_abs + 0xFFFC; //as per docs when cpu resets it looks here from program conter val
	uint8_t lsb = bus_read(address_abs);
	uint8_t msb = bus_read(address_abs + 1);

	program_counter = (msb << 8) | lsb;

	//reset internal vars
	address_abs = 0;
	address_rel = 0;
	fetched = 0;

	cycles_left = 0;
}

void cpu_irq(){ //interrupt request (IRQ) - can be ignored
	if(get_flag(interrupt_disable_bit) == 0){//do nothing if bit is set as it means we are not accepting interrupts
		//save current status to the stack
		bus_write(stack_ofset + stack_ptr, (program_counter >> 8) & 0x00FF); //save pc msb
		stack_ptr--;
		bus_write(stack_ofset + stack_ptr, program_counter & 0x00FF); //save pc lsb
		stack_ptr--;

		set_flag(break_bit, 0);
		set_flag(unused_bit, 1);
		set_flag(interrupt_disable_bit, 1);
		bus_write(stack_ofset + stack_ptr, status);
		stack_ptr--;

		//handle it
		address_abs = 0xFFFE;
		uint8_t lsb = bus_read(address_abs);
		uint8_t msb = bus_read(address_abs + 1);
		program_counter + (msb << 8) | lsb;

		cycles_left = 7;
	}
}

void cpu_nmi(){ //non maskable interrupt (NMI) - we cant ignore this one

		//save current status to the stack
		bus_write(stack_ofset + stack_ptr, (program_counter >> 8) & 0x00FF); //save pc msb
		stack_ptr--;
		bus_write(stack_ofset + stack_ptr, program_counter & 0x00FF); //save pc lsb
		stack_ptr--;

		set_flag(break_bit, 0);
		set_flag(unused_bit, 1);
		set_flag(interrupt_disable_bit, 1);
		bus_write(stack_ofset + stack_ptr, status);
		stack_ptr--;

		//handle it
		address_abs = 0xFFFA;
		uint8_t lsb = bus_read(address_abs);
		uint8_t msb = bus_read(address_abs + 1);
		program_counter + (msb << 8) | lsb;

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
		fetched = bus_read(address_abs);
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
	address_abs = bus_read(program_counter);
	program_counter++;
	address_abs = address_abs & 0x00FF; // we are reading from page 0 so we dont need MSB
	return 0;
}

uint8_t ADR_ZPX(){
	address_abs = bus_read(program_counter + x_reg);
	program_counter++;
	address_abs = address_abs & 0x00FF; // we are reading from page 0 so we dont need MSB
	return 0;
}

uint8_t ADR_ZPY(){
	address_abs = bus_read(program_counter + y_reg);
	program_counter++;
	address_abs = address_abs & 0x00FF; // we are reading from page 0 so we dont need MSB
	return 0;
}

uint8_t ADR_ABS(){
	uint8_t lsb = bus_read(program_counter);
	program_counter++;
	uint8_t msb = bus_read(program_counter);
	program_counter++;

	address_abs = (msb << 8) | lsb;
	return 0;
}

uint8_t ADR_ABX(){
	uint8_t lsb = bus_read(program_counter);
	program_counter++;
	uint8_t msb = bus_read(program_counter);
	program_counter++;

	address_abs = (msb << 8) | lsb;
	address_abs += x_reg;

	if ((address_abs & 0xFF00) != (msb << 8)) return 1; //if this makes us read an address on a different page we may need another clock cycle
	return 0;
}

uint8_t ADR_ABY(){
	uint8_t lsb = bus_read(program_counter);
	program_counter++;
	uint8_t msb = bus_read(program_counter);
	program_counter++;

	address_abs = (msb << 8) | lsb;
	address_abs += y_reg;

	if ((address_abs & 0xFF00) != (msb << 8)) return 1; //if this makes us read an address on a different page we may need another clock cycle
	return 0;
}


uint8_t ADR_IND(){
	uint8_t lsb = bus_read(program_counter);
	program_counter++;
	uint8_t msb = bus_read(program_counter);
	program_counter++;

	uint16_t ptr = (msb << 8) | lsb;

	address_abs = (bus_read(program_counter + 1) << 8) | bus_read(program_counter);
	
	if(lsb == 0x00FF){ //if reading the msb from the bus would make it overflow and switch page the 6502 would actually bug and start reading from the start of the current page, this behavior is emulated here. for further reading consult: https://www.nesdev.org/6502bugs.txt
		address_abs = (bus_read(ptr & 0xFF00) << 8) | bus_read(ptr);
	}else{//if no overflow then it behaves normaly
		address_abs = (bus_read(ptr + 1) << 8) | bus_read(ptr);
	}
	
	return 0;
}

uint8_t ADR_IZX(){
	uint8_t ptr = bus_read(program_counter);
	program_counter++;

	uint8_t lsb = bus_read(ptr + x_reg);
	uint8_t msb = bus_read(ptr + x_reg + 1);
	
	address_abs = (msb << 8) | lsb;
	return 0;
}

uint8_t ADR_IZY(){
	uint8_t ptr = bus_read(program_counter);
	program_counter++;

	uint8_t lsb = bus_read(ptr);
	uint8_t msb = bus_read(ptr + 1);
	
	address_abs = (msb << 8) | lsb;
	address_abs += y_reg;

	if ((address_abs & 0xFF00) != (msb << 8)) return 1;	
	return 0;
}

uint8_t ADR_REL(){
	address_rel = bus_read(program_counter);
	program_counter;
	
	if (address_rel & BIT(7)) address_rel |= 0xFF00;
	return 0;
}

/*Instructions  DOCS: https://www.nesdev.org/obelisk-6502-guide/reference.html
                      https://www.masswerk.at/6502/6502_instruction_set.html
                      ../doc/rockwell_r65c00_microprocessors.pdf
                      ../doc/instructionset.png

*/
uint8_t INST_RTI(){ //return from interrupt
	stack_ptr++;
	status = bus_read(stack_ofset + stack_ptr);
	status &= ~break_bit;
	status &= ~unused_bit;

	stack_ptr++;
	program_counter = (uint16_t)bus_read(stack_ofset + stack_ptr);
	stack_ptr++;
	program_counter |= (uint16_t)bus_read(stack_ofset + stack_ptr) << 8;

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
	bus_write(stack_ofset + stack_ptr, (program_counter >> 8) & 0x00FF); //save pc msb
	stack_ptr--;
	bus_write(stack_ofset + stack_ptr, program_counter & 0x00FF); //save pc lsb
	stack_ptr--;

	set_flag(break_bit, true);
	bus_write(stack_ofset + stack_ptr, status);
	stack_ptr--;
	set_flag(break_bit, false);

	program_counter = (uint16_t)bus_read(0xFFFE) | ((uint16_t)bus_read(0xFFFF) << 8);

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
		bus_write(address_abs, temp & 0x00FF);
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
	bus_write(address_abs, temp);
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
	bus_write(address_abs, temp);
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

	bus_write(stack_ofset + stack_ptr, (program_counter >> 8) & 0x00FF);
	stack_ptr--;
	bus_write(stack_ofset + stack_ptr, program_counter & 0x00FF);
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
		bus_write(address_abs, temp & 0x00FF);
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
	bus_write(stack_ofset + stack_ptr, accumulator);
	stack_ptr--;
	return 0;
}

uint8_t INST_PHP(){
	bus_write(stack_ofset + stack_ptr, status | break_bit | unused_bit);
	SetFlag(break_bit, 0);
	SetFlag(unused_bit, 0);
	stack_ptr--;
	return 0;
}

uint8_t INST_PLA(){
	stack_ptr++;
	accumulator = bus_read(stack_ofset + stack_ptr);
	set_flag(zero_bit, accumulator == 0);
	set_flag(negative_bit, accumulator & BIT(7));
	return 0;
}

uint8_t INST_PLP(){
	stack_ptr++;
	status = bus_read(stack_ofset + stack_ptr);
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
		bus_write(address_abs, temp & 0x00FF);
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
		bus_write(address_abs, temp & 0x00FF);
	}
	return 0;
}

uint8_t INST_RTS(){
	stack_ptr++;
	program_counter = bus_read(stack_ofset + stack_ptr);
	stack_ptr++;
	program_counter |= (uint16_t)bus_read(stack_ofset + stack_ptr) << 8;

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
	bus_write(address_abs, accumulator);
	return 0;
}	

uint8_t INST_STX(){
	bus_write(address_abs, x_reg);
	return 0;
}

uint8_t INST_STY(){
	bus_write(address_abs, y_reg);
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
