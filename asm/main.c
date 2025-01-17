#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "structs.h"
#include "definitions.h"
#include "assembler_functions.h"



int main(int argc, char* argv[])
{

	#define TEST "binom"
	#undef TEST
	#if defined(TEST)
	
		const char* directory = "..";
		sprintf(argv[1], "%s\\%s\\%s.asm", directory, TEST, TEST);
		sprintf(argv[2], "%s\\%s\\%s", directory, TEST, "imemin.txt");
		sprintf(argv[3], "%s\\%s\\%s", directory, TEST, "dmemin.txt");
	
	#endif
		// Creating pointers to input and output files
	FILE* fProgram = fopen(argv[1], "r");
	if (fProgram == NULL) {
		printf("Error: Could not open the program file: %s\n", argv[1]);
		return 1;  // Exit with error code
	}

	FILE* fImemin = fopen(argv[2], "w");
	if (fImemin == NULL) {
		printf("Error: Could not open the imemin file: %s\n", argv[2]);
		fclose(fProgram);  // Close any previously opened files
		return 1;
	}

	FILE* fDmemin = fopen(argv[3], "w");
	if (fDmemin == NULL) {
		printf("Error: Could not open the dmemin file: %s\n", argv[3]);
		fclose(fProgram);  // Close previously opened files
		fclose(fImemin);
		return 1;
	}

	printf("Files were opened successfully.\n");



	//global variabels and ptrs
	char* label_addresses_lst[DEPTH_OF_INSTRUCTIONS] = { 0 };
	int* data_memory = (int*)calloc(DEPTH_OF_MEMORY, sizeof(int));
	Instruction sInstruction;

	// first run - and creating a label list
	first_pass(fProgram, label_addresses_lst, data_memory);
	
	//returns the pointer to the beginning of the asm file
	fseek(fProgram, 0, 0);


	// Second run - handeling the regular instructions
	second_pass(fProgram, fImemin, label_addresses_lst, data_memory, &sInstruction);
	
	//flush the Imemin file
	fflush(fImemin);

	//load the memory data to Imemin file
	int depth = find_last_non_zero(data_memory, DEPTH_OF_MEMORY);
	load_data_to_file(fDmemin, data_memory, depth + 1);

	// clean the dynamic memory
	for (int i = 0; i < DEPTH_OF_INSTRUCTIONS; i++)
		free(label_addresses_lst[i]);
	
	
	//close the files
	fclose(fImemin);
	fclose(fDmemin);
	fclose(fProgram);
	printf("i ran");
	return 0;
}