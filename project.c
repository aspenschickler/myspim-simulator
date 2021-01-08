/*
 * Project members:
 * Zach Schickler
 * Stephen Maldonado
 * Yash Gharat
 */

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero) {
  if (A - B == 0) //checks for zero for branch
    *Zero = 1;
  else
    *Zero = 0;

	if (ALUControl == 0x0) { // Addition
		*ALUresult = A + B;
	}
	else if (ALUControl == 0x1) { // Subtraction
		*ALUresult = A - B;
	}
	else if (ALUControl == 0x2){ // Set less than
		if ((int) A < (int) B)
			*ALUresult = 1;
		else
			*ALUresult = 0;
	}
	else if (ALUControl == 0x3){ // Set less than unsigned
		if (A < B) //if less than
			*ALUresult = 1;
		else //otherwise
			*ALUresult = 0;
	}
	else if (ALUControl == 0x4){ // And
		*ALUresult = A & B;
	}
	else if (ALUControl == 0x5){ // Or
		*ALUresult = A | B;
	}
	else if (ALUControl == 0x6){ // Shift extended value left by 16 bits
		*ALUresult = B << 16;
	}
	else if (ALUControl == 0x7){ // R-type Instruction
    // Use "~" instead of "~" since it's a bitwise flip
		*ALUresult = ~A;
	}

	return;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction) {
  if (PC % 4 != 0) // If not word aligned, error
    return 1; //halts

    // Fetch next instruction from Mem. Important to bitwise shift PC by 2 first
    *instruction = Mem[PC >> 2];
    return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec) {
  // Isolating different bit sections of instruction
  *op = (instruction & 0xfc000000) >> 26;
  *r1 = (instruction & 0x03e00000) >> 21;
  *r2 = (instruction & 0x001f0000) >> 16;
  *r3 = (instruction & 0x0000f800) >> 11;
  *funct = instruction & 0x0000003f;
  *offset = instruction & 0x0000ffff;
  *jsec = instruction & 0x03ffffff;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls) {

  if (op == 0x0) { // Add, Sub, And, Or, Slt, & Sltu
    controls->RegDst = 1;
  	controls->Jump = 0;
  	controls->Branch = 0;
  	controls->MemRead = 0;
  	controls->MemtoReg = 0;
  	controls->ALUOp = 7;
  	controls->MemWrite = 0;
  	controls->ALUSrc = 0;
  	controls->RegWrite = 1;
    return 0;
  }
  else if (op == 0x2) { // Jump
    controls->RegDst = 2;
  	controls->Jump = 1;
  	controls->Branch = 2;
  	controls->MemRead = 0;
  	controls->MemtoReg = 2;
  	controls->ALUOp = 0;
  	controls->MemWrite = 0;
  	controls->ALUSrc = 2;
  	controls->RegWrite = 0;
    return 0;
  }
  else if (op == 0x4) { // Beq
    controls->RegDst = 2;
    controls->Jump = 0;
  	controls->Branch = 1;
  	controls->MemRead = 0;
  	controls->MemtoReg = 2;
  	controls->ALUOp = 1;
  	controls->MemWrite = 0;
  	controls->ALUSrc = 2;
  	controls->RegWrite = 0;
    return 0;
  }
  else if (op == 0x8) { // Addi
    controls->RegDst = 0;
  	controls->Jump = 0;
  	controls->Branch = 0;
  	controls->MemRead = 0;
  	controls->MemtoReg = 0;
  	controls->ALUOp = 0;
  	controls->MemWrite = 0;
  	controls->ALUSrc = 1;
  	controls->RegWrite = 1;
    return 0;
  }
  else if (op == 0xa) { // Slti
    controls->RegDst = 0;
  	controls->Jump = 0;
  	controls->Branch = 0;
  	controls->MemRead = 0;
  	controls->MemtoReg = 0;
  	controls->ALUOp = 2;
  	controls->MemWrite = 0;
  	controls->ALUSrc = 1;
  	controls->RegWrite = 1;
    return 0;
  }
  else if (op == 0xb) { // Sltiu
    controls->RegDst = 0;
  	controls->Jump = 0;
  	controls->Branch = 0;
  	controls->MemRead = 0;
  	controls->MemtoReg = 0;
  	controls->ALUOp = 3;
  	controls->MemWrite = 0;
  	controls->ALUSrc = 1;
  	controls->RegWrite = 1;
    return 0;
  }
  else if (op == 0xf) { // Lui
    controls->RegDst = 0;
  	controls->Jump = 0;
  	controls->Branch = 0;
  	controls->MemRead = 0;
  	controls->MemtoReg = 0;
  	controls->ALUOp = 6;
  	controls->MemWrite = 0;
  	controls->ALUSrc = 1;
  	controls->RegWrite = 1;
    return 0;
  }
  else if (op == 0x23) { // Lw
    controls->RegDst = 0;
  	controls->Jump = 0;
  	controls->Branch = 0;
  	controls->MemRead = 1;
  	controls->MemtoReg = 1;
  	controls->ALUOp = 0;
  	controls->MemWrite = 0;
  	controls->ALUSrc = 1;
  	controls->RegWrite = 1;
    return 0;
  }
  else if (op == 0x2b) { // Sw
    controls->RegDst = 0;
  	controls->Jump = 0;
  	controls->Branch = 0;
  	controls->MemRead = 0;
  	controls->MemtoReg = 0;
  	controls->ALUOp = 0;
  	controls->MemWrite = 1;
  	controls->ALUSrc = 1;
  	controls->RegWrite = 0;
    return 0;
  }
  else {
    return 1;
  }
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2) {
	*data1 = Reg[r1]; //reads in data from first requested register
	*data2 = Reg[r2]; //reads in data from second requested register
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value) {
	if((offset >> 15) == 1)
		*extended_value = offset | 0xffff0000; // Negative case
	else
		*extended_value = offset & 0x0000ffff; // Positive case
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero) {
	// Using ALUOp didn't appear to save changes, so I had to make another variable in the method and then set it equal to ALUOp.
	// Doing this seemed to fix it.
	unsigned char ALUOp_dupe = ALUOp;
	if (ALUOp == 0x7) { // If ALUOp is R-Type
		switch(funct){
			case 0x20:	// Add
				ALUOp_dupe = 0x0;
				break;
			case 0x22:	// Subtraction
				ALUOp_dupe = 0x1;
				break;
			case 0x24:	// And
				ALUOp_dupe = 0x4;
				break;
			case 0x25:	// Or
				ALUOp_dupe = 0x5;
				break;
			case 0x2a:	// Set less than signed
				ALUOp_dupe = 0x2;
				break;
			case 0x2b:	// Set less than unsigned
				ALUOp_dupe = 0x3;
				break;
			default:	// Halt
				return 1;
		}
	}

  // Determining whether or not to use the extended value based on ALUSrc
  if (ALUSrc == 1)
    ALU(data1, extended_value, ALUOp_dupe, ALUresult, Zero);
  else
    ALU(data1, data2, ALUOp_dupe, ALUresult, Zero);

	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem) {
  // Reading before writing
  // Initially, I tried checking if ALUresult % 4 == 0 && ALUresult < 65536 were true before reading and writing, but changes
  // weren't showing up in the registers. I switched to checking individually, and it started working
	if(MemRead  == 1) {
		if( ALUresult % 4 == 0 && ALUresult < 65536 )
			*memdata = Mem[ALUresult >> 2];
		else
		{
			return 1; //otherwise halts
		}
  }

	if(MemWrite == 1) {
		if( ALUresult % 4 == 0 && ALUresult < 65536 ) //checks validity
			Mem[ALUresult >> 2] = data2;
		else
		{
		  return 1; //halts if incorrect
	  	}
  }

	return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg) {
	if(RegWrite == 1){
		if(MemtoReg == 0){
			if(RegDst == 1) // R-Type Instruction
				Reg[r3] = ALUresult;
			else // I-Type Instruction
				Reg[r2] = ALUresult;
  		}
		else if(MemtoReg == 1){
			Reg[r2] = memdata;
		}
	}
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC) {
  // First, do the advancing
	*PC = *PC + 4; //adds four because byte-addressable
  // Next, check for a jump
  if(Jump == 1)
    *PC = (*PC & 0xf0000000) | (jsec << 2);
  // Finally check for branching
	if(Zero == 1 && Branch == 1)
		*PC = *PC + (extended_value << 2);
}
