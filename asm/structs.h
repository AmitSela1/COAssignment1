#ifndef _STRUCTS
#define _STRUCTS
#include "definitions.h"
//this file contains the general stucts used by the assembler and the simulator

//used for the diiferent opcodes defined used by SIMP
typedef enum _OpCode {
	ADD = 0,
	SUB,
	MAC,
	AND,
	OR,
	XOR,
	SLL,
	SRA,
	SRL,
	BEQ,
	BNE,
	BLT,
	BGT,
	BLE,
	BGE,
	JAL,
	LW,
	SW,
	RETI,
	IN,
	OUT,
	HALT,
} OpCode;

//this is an insruction
typedef struct _Instruction {
	enum OpCode opcode;
	int rd;
	int rs;
	int rt;
	int rm;
	int immediate1;
	int immediate2;
} Instruction;



/*there are 4 types of lines in each assembly file:
* REGULAR_INST- A regular intruction
*PSEUDO- a .Word 
*LABEL- a label
*COMMENT- just a writen comment by the writer.
*/
typedef enum _Line_Type {
	REGULAR_INST,
	PSEUDO,
	LABEL,
	COMMENT,
} Line_Type;

//used for .Word
typedef struct _Word {
	int address;
	int value;
} Word;

//the Registers that are declared in register file
typedef enum _Register {
	ZERO = 0,
	IMM1,
	IMM2,
	V0,
	A0,
	A1,
	A2,
	T0,
	T1,
	T2,
	S0,
	S1,
	S2,
	GP,
	SP,
	RA,
} Register;

//the IO HW Registers that are declared in register file
typedef enum _IO_HW_Register { 
	Irq0Enable = 0,
	Irq1Enable,
	Irq2Enable,
	Irq0Status,
	Irq1Status,
	Irq2Status,
	IrqHandler,
	IrqReturn,
	Clks,
	Leds,
	Display7Seg,
	TimerEnable,
	TimerCurrent,
	TimerMax,
	DiskCmd,
	DiskSector,
	DiskBuffer,
	DiskStatus,
	RESERVED1,
	RESERVED2,
	MonitorADDR,
	MonitorDATA,
	MonitorCMD,
} IO_HW_Register;

#endif