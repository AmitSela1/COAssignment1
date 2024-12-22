#ifndef SIM_FUNCTIONS
#define SIM_FUNCTIONS
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "definitions.h"
#include "structs.h"

void get_instruction(TYPE_OF_INSTRUCTION instruction_bytes, Instruction* inst);
void read_and_load_instruction(FILE*, TYPE_OF_INSTRUCTION*);
void read_and_load_data(FILE*, DATA*);
DATA get_numeric_value_from_hex_data(const char* field);
int read_cur_line(FILE* input, char* line);
void execute_inst(Instruction* instruction, unsigned short* PC, int* registers, int* IOregisters, DATA* memory, BOOL* halt, BOOL* in_interrupt);
long long int* read_irq2_cycles(FILE* input);
void update_line_in_trace(int PC, TYPE_OF_INSTRUCTION instruction, DATA* registers, FILE* output);
void get_string_from_io_reg(IO_HW_Register code, char str[]);
void load_data_to_file(FILE* output_file, DATA* output_data, int line_num);
void load_monitor_txt(FILE* output_stream, char* output_data);
void load_monitor_yuv(FILE* binary_stream, char* output_data);
int find_last_non_zero(DATA* data_memory, int max_size);
void handel_timer(DATA* IOregisters_lst);
void handel_disk(DATA* IOregisters_lst, short* disk_cnt, DATA* disk_mem, DATA* data_mem);
void handel_monitor(DATA* IOregisters_lst, char* monitor_mem);
void handel_interrupt(DATA* IOregisters_lst, BOOL* irq, BOOL* interrupt_input, unsigned short* PC);
void handel_leds(Instruction* cur_inst, DATA* registers_lst, long long int* cycles_cnt, DATA* IOregisters_lst, FILE* leds);
void handel_disply7seg(Instruction* cur_inst, DATA* registers_lst, long long int* cycles_cnt, DATA* IOregisters_lst, FILE* display7seg);
void handel_hwtrace(Instruction* cur_inst, long long int* cycles_cnt, DATA* IOregisters_lst, DATA* registers_lst, FILE* hwregtrace);
void load_const_val_to_reg_list(DATA* registers_lst, Instruction* cur_inst);
void handel_irq2(long long int* cycles_cnt, long long int* irq_2_input, DATA* IOregisters_lst);
void finish_up_line(long long int* cycles_cnt, FILE* leds, FILE* display7seg, FILE* trace, FILE* hwregtrace);
#endif
