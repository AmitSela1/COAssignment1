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
 
	//creatig ptrs to input and output files
	FILE* fProgram = fopen(argv[1], "r");
	FILE* fImemin = fopen(argv[2], "w");
	FILE* fDmemin = fopen(argv[3], "w");
	

	//global variabels and ptrs
	int instruction_cnt = 0;
	Line_Type Type_of_inst = 0;
	char* temp_label = NULL;
	char* label_addresses_lst[DEPTH_OF_INSTRUCTIONS] = {0};
	int* data_memory = (int*)calloc(DEPTH_OF_MEMORY, sizeof(int));
	char cur_line[MAX_LINE_LENGTH];
	char cur_cleaned_line[MAX_LINE_LENGTH];
	Instruction sInstruction;
	
	// First pass: Read the program and create a list of labels with their corresponding addresses
	int i = 0;
	while (!feof(fProgram)) { // Continue until the end of the file
		if (read_cur_line(fProgram, cur_line) == 0) continue; // reads the current line from the *.asm file and skip to the next iteration if the line is empty
		
		// Check if the current line contains a label or a label with an instruction
		BOOL label_and_inst = 0;
		clean_line(cur_line, cur_cleaned_line); // Remove unnecessary spaces and format the line
		temp_label = get_label(cur_cleaned_line, &label_and_inst); // Extract label if present and check if it's inline with an instruction
		if (strlen(cur_cleaned_line) == 0) continue; // Skip the iteration if the cleaned line is empty
		if (temp_label) {
			label_addresses_lst[instruction_cnt] = temp_label;// Add the label to the label-address list
			instruction_cnt += (int)label_and_inst; // Increment the instruction count if the label is inline with an instruction
		}
		else if (found_pseudo(cur_cleaned_line))
		{
			manage_pseudo(cur_cleaned_line, data_memory); // Handle pseudo-instructions and update data memory as needed
			
		}
		else if (line_is_comment(cur_cleaned_line))
		{
			 // Skip processing if the line is a comment
		}
		else {
			// Increment the instruction count for valid instructions
			instruction_cnt++;
		}
	}
	
	//returns the pointer to the beginning of the asm file
	fseek(fProgram, 0, 0);

	// Second run - handeling the regular instructions 
	int line_counter = 1;
	while (!feof(fProgram)) {                      // feof is a function that checks if the end of a file has been reached.      
		if (read_cur_line(fProgram, cur_line) == 0) continue; // empty line, not interesting
		//clean_line(cur_line, cur_cleaned_line);
		Tiype_of_inst = get_line_type(cur_line, &sInstruction, label_addresses_lst, fImemin);

		switch (Type_of_inst)
		{
		case REGULAR_INST:
			//get_and_encode_instruction(cur_cleaned_line, &sInstruction, label_addresses_lst, fImemin);
			line_counter++;
		case PSEUDO:
		case LABEL:
		case COMMENT:
			break;
		}
	}
	fflush(fImemin);

	//load the memory data to Imemin file
	int depth = find_last_non_zero(data_memory, DEPTH_OF_MEMORY);
	load_data_to_file(fDmemin, data_memory, depth + 1);

	// clean the dynamic memory
	for (int i = 0; i < DEPTH_OF_INSTRUCTIONS; i++)
		free(label_addresses_lst[i]);

	fclose(fImemin);
	fclose(fDmemin);
	fclose(fProgram);

	return 0;
}