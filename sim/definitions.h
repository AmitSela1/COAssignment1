#ifndef _DEFINITIONS
#define _DEFINITIONS

// Project defenitions
#define DEPTH_OF_INSTRUCTIONS 4096
#define DEPTH_OF_MEMORY 4096

#define MAX_LINE_LENGTH 500
#define MAX_LABLE_LENGTH 50

#define SIZE_OF_INSTRUCTION_BITS 48
#define SIZE_OF_INSTRUCTION_CHARS 12

#define MEMORY_SIZE_BITS 32
#define MEMORY_SIZE_CHARS 8

#define NUM_OF_REGISTERS 16
#define NUM_OF_IO_REGISTERS 23

#define SCREEN_X_AXIS 256
#define SCREEN_Y_AXIS 256

#define NUM_OF_DISK_SECTORS 128
#define WORDS_IN_SECTOR 128
#define SECTOR_SIZE_IN_BYTES 512
#define CYCLE_LENGTH_OF_DISK_OPERATION 1024

// General defenitions
#define BOOL int
#define TRUE 1
#define FALSE 0
#define TYPE_OF_INSTRUCTION unsigned long long int
#define DATA unsigned int
#define DO_NOTHING 0
#define READ 1
#define WRITE 2
#define FREE 0
#define BUSY 1
#define ENABLED 1
#define DISABLED 0

// Instructions feilds
#define OPCODE_SIZE_IN_BITS 8
#define	RD_SIZE_IN_BITS 4
#define RS_SIZE_IN_BITS 4
#define RT_SIZE_IN_BITS 4
#define RM_SIZE_IN_BITS 4
#define IMM1_SIZE_IN_BITS 12
#define IMM2_SIZE_IN_BITS 12

#define OPCODE_MASK 0xff0000000000
#define RD_MASK		0x00f000000000
#define RS_MASK		0x000f00000000
#define RT_MASK		0x0000f0000000
#define RM_MASK		0x00000f000000
#define IMM1_MASK	0x000000fff000
#define IMM2_MASK	0x000000000fff
#endif

