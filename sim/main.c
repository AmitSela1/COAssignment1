#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "structs.h"
#include "definitions.h"
#include "simulator_function.h"


int main(int argc, char* argv[])
{
#define TEST "mulmat"
#undef TEST
#if defined(TEST)
    
    //save pthhs to file name in argv[i]
    const char* directory = "..";       
    sprintf(argv[1], "%s\\%s\\%s", directory, TEST, "imemin.txt");
    sprintf(argv[2], "%s\\%s\\%s", directory, TEST, "dmemin.txt");
    sprintf(argv[3], "%s\\%s\\%s", directory, TEST, "diskin.txt");
    sprintf(argv[4], "%s\\%s\\%s", directory, TEST, "irq2in.txt");
    sprintf(argv[5], "%s\\%s\\%s", directory, TEST, "dmemout.txt");
    sprintf(argv[6], "%s\\%s\\%s", directory, TEST, "regout.txt");
    sprintf(argv[7], "%s\\%s\\%s", directory, TEST, "trace.txt");
    sprintf(argv[8], "%s\\%s\\%s", directory, TEST, "hwregtrace.txt");
    sprintf(argv[9], "%s\\%s\\%s", directory, TEST, "cycles.txt");
    sprintf(argv[10], "%s\\%s\\%s", directory, TEST, "leds.txt");
    sprintf(argv[11], "%s\\%s\\%s", directory, TEST, "display7seg.txt");
    sprintf(argv[12], "%s\\%s\\%s", directory, TEST, "diskout.txt");
    sprintf(argv[13], "%s\\%s\\%s", directory, TEST, "monitor.txt");
    sprintf(argv[14], "%s\\%s\\%s", directory, TEST, "monitor.yuv");
#endif
    printf("simulator is compiled");
    // opening input files
    FILE* imemin = fopen(argv[1], "r");
    FILE* dmemin = fopen(argv[2], "r");
    FILE* diskin = fopen(argv[3], "r");
    FILE* irq2in = fopen(argv[4], "r");

    // opening output files
    FILE* dmemout     = fopen(argv[5], "w");
    FILE* regout      = fopen(argv[6], "w");
    FILE* trace       = fopen(argv[7], "w");
    FILE* hwregtrace  = fopen(argv[8], "w");
    FILE* cycles      = fopen(argv[9], "w");
    FILE* leds        = fopen(argv[10], "w");
    FILE* display7seg = fopen(argv[11], "w");
    FILE* diskout     = fopen(argv[12], "w");
    FILE* monitor     = fopen(argv[13], "w");
    FILE* monitor_yuv = fopen(argv[14], "wb");
    
    FILE* output_files[] = { imemin, dmemin, diskin, irq2in, dmemout, regout, trace, hwregtrace, cycles, leds, display7seg, diskout, monitor, monitor_yuv };

    //pointers
    TYPE_OF_INSTRUCTION* inst_mem = (TYPE_OF_INSTRUCTION*)malloc(sizeof(TYPE_OF_INSTRUCTION) * DEPTH_OF_INSTRUCTIONS); //Allocates a block of memory of the specified size
    DATA* data_mem = (DATA*)calloc(DEPTH_OF_MEMORY, sizeof(DATA)); //Allocates and initializes the memory to zero
    char* monitor_mem = (char*)calloc(SCREEN_X_AXIS * SCREEN_Y_AXIS, sizeof(char));
    DATA* disk_mem = (DATA*)calloc(NUM_OF_DISK_SECTORS* WORDS_IN_SECTOR, sizeof(DATA));

    //read and load input files
    long long int* irq_2_input = read_irq2_cycles(irq2in);
    read_and_load_data(diskin, disk_mem);
    read_and_load_instruction(imemin, inst_mem);
    read_and_load_data(dmemin, data_mem);
   
    //global variabels
    unsigned short PC = 0; //16-bit unsigned integer initialized to 0
    Instruction cur_inst; //storing the current instruction
    BOOL halt = FALSE; //A boolean flag indicating whether the simulation should stop (TRUE or FALSE).
    BOOL irq = 0;
    BOOL interrupt_input = 0;
    
    //register list
    DATA registers_lst[NUM_OF_REGISTERS] = { 0 };
    DATA IOregisters_lst[NUM_OF_IO_REGISTERS] = { 0 };

    //counters
    long long int cycles_cnt = 0;
    short disk_cnt = 1024;
    registers_lst[ZERO] = 0;
    while (!halt && PC < DEPTH_OF_INSTRUCTIONS)//continue as long as hult command was not given and there is an istruction to execute 
    {

        // get instruction
        fetch_instruction(inst_mem[PC], &cur_inst);
        
        // load constants values to reg list
        load_imm_to_reg_list(registers_lst,&cur_inst);
   
        //write current line in trace befor executing an instruction
        update_line_in_trace(PC, inst_mem[PC], registers_lst, trace);

        // execute instruction 
        execute_inst(&cur_inst, &PC, registers_lst, IOregisters_lst, data_mem, &halt, &interrupt_input); //updating the program state (PC, registers, memory, etc.).

        //write current line in trace befor executing an instruction
        load_imm_to_reg_list(registers_lst, &cur_inst);
      
        //if the opcode are out or in we write current line in hwtrace after executing an instruction
        handel_hwtrace(&cur_inst, &cycles_cnt, IOregisters_lst, registers_lst, hwregtrace);

        // handel timer
        handel_timer(IOregisters_lst);
       
        // handel disk 
        handel_disk(IOregisters_lst, &disk_cnt, disk_mem, data_mem);
       
        // handel irq2
        handel_irq2(&cycles_cnt, irq_2_input, IOregisters_lst);
   
        // handel monitor
        handel_monitor(IOregisters_lst, monitor_mem);

        //handel interrupt 
        handel_interrupt(IOregisters_lst, &irq, &interrupt_input, &PC);
        
        //handel leds
        handel_leds(&cur_inst, registers_lst, &cycles_cnt, IOregisters_lst,leds);

        //handel display7seg
        handel_disply7seg(&cur_inst, registers_lst, &cycles_cnt, IOregisters_lst, display7seg);

        // finish_up_line
        finish_up_line(&cycles_cnt, leds, display7seg, trace, hwregtrace);

    }
    
    int memory_depth = find_last_non_zero(data_mem, DEPTH_OF_MEMORY); //Finds the last non-zero element in the memory array to optimize file writing.
    int disk_depth = find_last_non_zero(disk_mem, NUM_OF_DISK_SECTORS*WORDS_IN_SECTOR);

    // loads output files
    load_data_to_file(dmemout, data_mem, memory_depth + 1);                  // load dmemout
    load_data_to_file(regout, registers_lst + 3, NUM_OF_REGISTERS - 3);      // load regout
    fprintf(cycles, "%ld\n", cycles_cnt);                                    // load cycles
    load_data_to_file(diskout, disk_mem, disk_depth + 1);                    // load diskout
    load_monitor_txt(monitor, monitor_mem);                                  // load monitor.txt
    load_monitor_yuv(monitor_yuv, monitor_mem);                              // load monitor.yuv


    // close all files
    for (int i = 0; i < sizeof(output_files) / sizeof(output_files[0]); ++i) {
        if (output_files[i]) {
            fclose(output_files[i]);
        }
    }
  
    //free memory
    free(irq_2_input);
    free(inst_mem);
    free(data_mem);
    free(monitor_mem);
    free(disk_mem);
	printf("simulator is done");
    
}