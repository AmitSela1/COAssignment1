#ifndef _FUNCTIONS
#define _FUNCTIONS

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "structs.h"
#include "definitions.h"


void first_pass(FILE* input, char* labels[DEPTH_OF_INSTRUCTIONS], int* memory_data);
void second_pass(FILE* input, FILE* imemin, char* labels[DEPTH_OF_INSTRUCTIONS], int* memory_data, Instruction* sInstruction);
int read_cur_line(FILE* input, char* line);
int get_and_encode_instruction(const char* line, Instruction* output, char* labels[DEPTH_OF_INSTRUCTIONS], FILE* imemin);
Line_Type get_line_type(char* cur_line, Instruction* sInstruction, char* label_addresses_lst[DEPTH_OF_INSTRUCTIONS], FILE* fImemin);
int clean_line(const char* to_fix, char* fixed);
OpCode get_op(const char* opcode);
Register get_reg(const char* reg);
void lower_str(const char* input, char* output);
int get_num_val(const char* field, char* labels[DEPTH_OF_INSTRUCTIONS]);
char* get_label(char* cleaned_line, BOOL* inline_label);
void manage_pseudo(const char* line, int* memory_data);
void load_data_to_file(FILE* output_stream, DATA* output_data, int size);
int find_last_non_zero(DATA* data_memory, int max_size);
BOOL found_label(const char* line);
BOOL found_pseudo(const char* line);
BOOL line_is_comment(const char* line);
#endif