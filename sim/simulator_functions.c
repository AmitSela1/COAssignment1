#include "simulator_function.h"

/// <summary>
/// loads current instruction from memory
/// </summary>
/// <param name="cur_inst_mem"></param>
/// <param name="cur_inst"></param>
void get_instruction(TYPE_OF_INSTRUCTION cur_inst_mem, Instruction* cur_inst)
{
	int idx = SIZE_OF_INSTRUCTION_BITS;
	
	idx -= OPCODE_SIZE_IN_BITS;
	cur_inst->opcode = (OpCode)((cur_inst_mem & OPCODE_MASK) >> idx);
	
	idx -= RD_SIZE_IN_BITS;
	cur_inst->rd = (cur_inst_mem & RD_MASK) >> idx;

	idx -= RS_SIZE_IN_BITS;
	cur_inst->rs = (cur_inst_mem & RS_MASK) >> idx;

	idx -= RT_SIZE_IN_BITS;
	cur_inst->rt = (cur_inst_mem & RT_MASK) >> idx;

	idx -= RM_SIZE_IN_BITS;
	cur_inst->rm = (cur_inst_mem & RM_MASK) >> idx;
	
	idx -= IMM1_SIZE_IN_BITS;
	cur_inst->immediate1 = (cur_inst_mem & IMM1_MASK) >> idx;
	if (cur_inst->immediate1 & 0x000000000800) 
		cur_inst->immediate1 ^= 0xfffffffff000;

	idx -= IMM2_SIZE_IN_BITS;
	cur_inst->immediate2 = (cur_inst_mem & IMM2_MASK) >> idx;
	if (cur_inst->immediate2 & 0x000000000800)
		cur_inst->immediate2 ^= 0xfffffffff000;
}

/// <summary>
/// reads cuurent instuction from imemim to memory
/// </summary>
/// <param name="input"></param>
/// <param name="output"></param>
void read_and_load_instruction(FILE* input_file, TYPE_OF_INSTRUCTION* inst) {
	char line[MAX_LINE_LENGTH];
	int i = 0;
	while (read_cur_line(input_file, line)) {
		sscanf(line, "%llx", &inst[i++]);
	}
}

/// <summary>
/// reads diskin or dmemin and loads to memory
/// </summary>
/// <param name="input"></param>
/// <param name="output"></param>
void read_and_load_data(FILE* input_file, DATA* output) {
	const char line[MAX_LINE_LENGTH];
	int i = 0;
	while (read_cur_line(input_file, line)) {
		output[i++] = get_numeric_value_from_hex_data(line);
	}
}

/// <summary>
/// helper function for read and load data
/// </summary>
/// <param name="field"></param>
/// <returns></returns>
DATA get_numeric_value_from_hex_data(const char* field) {
	DATA retval;
	sscanf(field, "%x", &retval);
	return retval;
}

/// <summary>
/// loads the data to output file
/// </summary>
/// <param name="output_file"></param>
/// <param name="output_data"></param>
/// <param name="line_num"></param>
void load_data_to_file(FILE* output_file, DATA* output_data, int line_num) {
	for (int i = 0; i < line_num; i++) {
		// Check if fprintf successfully writes the data. If not, break the loop.
		if (fprintf(output_file, "%08X\n", output_data[i]) < 0) {
			fprintf(stderr, "Error writing data to file.\n");
			break;
		}
	}
}

/// <summary>
/// load the monitor.txt file
/// </summary>
/// <param name="output_stream"></param>
/// <param name="output_data"></param>
void load_monitor_txt(FILE* output_stream, char* output_data)
{
	int screen_size = SCREEN_X_AXIS * SCREEN_Y_AXIS;
	for (int i = 0; i < screen_size;  i++)
	{
		fprintf(output_stream, "%02hhX\n", output_data[i]);
	}
}

/// <summary>
/// load the monitor.yuv file
/// </summary>
/// <param name="binary_stream"></param>
/// <param name="output_data"></param>
void load_monitor_yuv(FILE* binary_stream, char* output_data)
{
	fwrite(output_data, 1, NUM_OF_DISK_SECTORS * SECTOR_SIZE_IN_BYTES, binary_stream);
}

/// <summary>
///  finds the last non-zero address in memory
/// </summary>
/// <param name="data_memory"></param>
/// <param name="max_size"></param>
/// <returns></returns>
int find_last_non_zero(DATA* data_memory, int max_size)
{
	int last_non_zero_index = 0;
	for (int i = 0; i < max_size; i++)
	{
		if (data_memory[i] != 0)
		{
			last_non_zero_index = i;
		}
	}
	return last_non_zero_index;
}

/// <summary>
/// reads a single line from a given input file
/// </summary>
/// <param name="input"> - file to read lines from</param>
/// <param name="line"> - line will be written here</param>
/// <returns>number of bytes in output line</returns>
int read_cur_line(FILE* input_file, char* line) {
	if (input_file == NULL) {
		line[0] = '\0';
		return 0;
	}

	int i = 0;
	int c; 

	while ((c = fgetc(input_file)) != EOF && c != '\n' && c != '\0') {
		line[i++] = (char)c;
	}

	line[i] = '\0'; 
	return i; 
}

/// <summary>
/// load the value of rd
/// </summary>
/// <param name="rd"></param>
/// <param name="value"></param>
/// <param name="registers"></param>
void loads_rd(int rd, int value, int* registers)
{
	if (rd != ZERO && rd != IMM1 && rd != IMM2)
		registers[rd] = value;
}

/// <summary>
/// execute the current instuction
/// </summary>
/// <param name="cur_inst"></param>
/// <param name="PC"></param>
/// <param name="registers_lst"></param>
/// <param name="IOregisters_lst"></param>
/// <param name="memory"></param>
/// <param name="halt"></param>
/// <param name="in_interrupt"></param>
void execute_inst(Instruction* cur_inst, unsigned short* PC, int* registers_lst, int* IOregisters_lst, DATA* memory, BOOL* halt, BOOL* in_interrupt)
{
	unsigned int temp = 0;
	int val = 0;

	switch (cur_inst->opcode)
	{
		case (ADD):
			val = registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt] + registers_lst[cur_inst->rm];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (SUB):
			val = registers_lst[cur_inst->rs] - registers_lst[cur_inst->rt] - registers_lst[cur_inst->rm];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (MAC):
			val = registers_lst[cur_inst->rs] * registers_lst[cur_inst->rt] + registers_lst[cur_inst->rm];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (AND):
			val = registers_lst[cur_inst->rs] & registers_lst[cur_inst->rt] & registers_lst[cur_inst->rm];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (OR):
			val = registers_lst[cur_inst->rs] | registers_lst[cur_inst->rt] | registers_lst[cur_inst->rm];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (XOR):
			val = registers_lst[cur_inst->rs] ^ registers_lst[cur_inst->rt] ^ registers_lst[cur_inst->rm];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (SLL):
			val = registers_lst[cur_inst->rs] << registers_lst[cur_inst->rt];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (SRA):
			val = registers_lst[cur_inst->rs] >> registers_lst[cur_inst->rt];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (SRL):
			temp = (unsigned)registers_lst[cur_inst->rs];
			temp >>= registers_lst[cur_inst->rt];
			registers_lst[cur_inst->rd] = (signed)temp;
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (BEQ):
			if (registers_lst[cur_inst->rs] == registers_lst[cur_inst->rt])
				*PC = (registers_lst[cur_inst->rm] & 0xfff);
			else (*PC)++;
			break;

		case (BNE):
			if (registers_lst[cur_inst->rs] != registers_lst[cur_inst->rt])
				*PC = (registers_lst[cur_inst->rm] & 0xfff);
			else (*PC)++;
			break;

		case (BLT):
			if (registers_lst[cur_inst->rs] < registers_lst[cur_inst->rt])
				*PC = (registers_lst[cur_inst->rm] & 0xfff);
			else (*PC)++;
			break;

		case (BGT):
			if (registers_lst[cur_inst->rs] > registers_lst[cur_inst->rt])
				*PC = (registers_lst[cur_inst->rm] & 0xfff);
			else (*PC)++;
			break;

		case (BLE):
			if (registers_lst[cur_inst->rs] <= registers_lst[cur_inst->rt])
				*PC = (registers_lst[cur_inst->rm] & 0xfff);
			else (*PC)++;
			break;

		case (BGE):
			if (registers_lst[cur_inst->rs] >= registers_lst[cur_inst->rt])
				*PC = (registers_lst[cur_inst->rm] & 0xfff);
			else (*PC)++;
			break;

		case (JAL):
			val = *PC + 1;
			loads_rd(cur_inst->rd, val, registers_lst);
			*PC = registers_lst[cur_inst->rm] & 0xfff;
			break;

		case (LW):
			val = memory[registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt]] + registers_lst[cur_inst->rm];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (SW):
			memory[registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt]] = registers_lst[cur_inst->rm] + registers_lst[cur_inst->rd];
			(*PC)++;
			break;

		case (RETI):
			*PC = IOregisters_lst[IrqReturn];
			*in_interrupt = FALSE;
			break;

		case (IN):
			if (registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt] == MonitorCMD)
				val = 0;
			val = IOregisters_lst[registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt]];
			loads_rd(cur_inst->rd, val, registers_lst);
			(*PC)++;
			break;

		case (OUT):
			IOregisters_lst[registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt]] = registers_lst[cur_inst->rm];
			(*PC)++;
			break;

		case (HALT):
			*halt = 1;
			(*PC)++;
			break;
	}
}

/// <summary>
/// reads irq2 activation cycles
/// </summary>
/// <param name="input"></param>
/// <param name="output"></param>
long long int* read_irq2_cycles(FILE* input) {
	char line[MAX_LINE_LENGTH];
	int size = 1000; // Initial size
	long long int* output = (long long int*)malloc(size * sizeof(long long int));
	if (!output) return NULL; // Check if malloc failed

	int cur_size = 0; // Current number of elements stored
	while (read_cur_line(input, line) > 0) {
		long long int num = 0;
		sscanf(line, "%lld", &num);
		output[cur_size++] = num;

		// Check if we need to expand the array
		if (cur_size == size) {
			size *= 2; // Double the size
			long long int* temp = (long long int*)realloc(output, size * sizeof(long long int));
			if (!temp) {
				free(output); // If realloc fails, free the originally allocated memory and return NULL
				return NULL;
			}
			output = temp; // Use the newly allocated memory
		}
	}

	// Adjust the size of the array to fit the number of elements exactly, plus one for the sentinel
	long long int* final_output = (long long int*)realloc(output, (cur_size + 1) * sizeof(long long int));
	if (!final_output) {
		free(output); // Handle realloc failure, though unlikely when shrinking
		return NULL;
	}
	final_output[cur_size] = -1; // Sentinel value

	return final_output;
}

/// <summary>
/// loads the current line to trace file
/// </summary>
/// <param name="PC"></param>
/// <param name="cur_inst"></param>
/// <param name="registers_lst"></param>
/// <param name="trace"></param>
void update_line_in_trace(int PC, TYPE_OF_INSTRUCTION cur_inst, DATA* registers_lst, FILE* trace)
{
	fprintf(trace, "%03X %012llX", PC, cur_inst);
	for (int i = 0; i < NUM_OF_REGISTERS; i++)
	{
		fprintf(trace, " %08x", registers_lst[i]);
	}
	fprintf(trace, "\n");
}

/// <summary>
/// covert io_reg to string
/// </summary>
/// <param name="code"></param>
/// <param name="str"></param>
void get_string_from_io_reg(IO_HW_Register code, char str[]) {
	switch (code) {
	case Irq0Enable:
		strcpy(str, "irq0enable");
		break;
	case Irq1Enable:
		strcpy(str, "irq1enable");
		break;
	case Irq2Enable:
		strcpy(str, "irq2enable");
		break;
	case Irq0Status:
		strcpy(str, "irq0status");
		break;
	case Irq1Status:
		strcpy(str, "irq1status");
		break;
	case Irq2Status:
		strcpy(str, "irq2status");
		break;
	case IrqHandler:
		strcpy(str, "irqhandler");
		break;
	case IrqReturn:
		strcpy(str, "irqreturn");
		break;
	case Clks:
		strcpy(str, "clks");
		break;
	case Leds:
		strcpy(str, "leds");
		break;
	case Display7Seg:
		strcpy(str, "display7seg");
		break;
	case TimerEnable:
		strcpy(str, "timerenable");
		break;
	case TimerCurrent:
		strcpy(str, "timercurrent");
		break;
	case TimerMax:
		strcpy(str, "timermax");
		break;
	case DiskCmd:
		strcpy(str, "diskcmd");
		break;
	case DiskSector:
		strcpy(str, "disksector");
		break;
	case DiskBuffer:
		strcpy(str, "diskbuffer");
		break;
	case DiskStatus:
		strcpy(str, "diskstatus");
		break;
	case RESERVED1:
		strcpy(str, "reserved1");
		break;
	case RESERVED2:
		strcpy(str, "reserved2");
		break;
	case MonitorADDR:
		strcpy(str, "monitoraddr");
		break;
	case MonitorDATA:
		strcpy(str, "monitordata");
		break;
	case MonitorCMD:
		strcpy(str, "monitorcmd");
		break;
	default:
		str[0] = '\0'; // Handle unknown codes
		break;
	}
}

/// <summary>
/// handel timer 
/// </summary>
/// <param name="IOregisters_lst"></param>
void handel_timer(DATA* IOregisters_lst)
{
	if (IOregisters_lst[TimerEnable])
	{
		if (IOregisters_lst[TimerCurrent] == IOregisters_lst[TimerMax])
		{
			IOregisters_lst[TimerCurrent] = 0;
			IOregisters_lst[Irq0Status] = 1;
		}
		else
			IOregisters_lst[TimerCurrent]++;
	}
}

/// <summary>
/// hande the disk drive
/// </summary>
/// <param name="IOregisters_lst"></param>
/// <param name="disk_cnt"></param>
/// <param name="disk_mem"></param>
/// <param name="data_mem"></param>
void handel_disk(DATA* IOregisters_lst,short* disk_cnt, DATA* disk_mem, DATA* data_mem)
{
	if (IOregisters_lst[DiskStatus] == FREE) {
            if (IOregisters_lst[DiskCmd])
            {
                IOregisters_lst[DiskStatus] = BUSY;
                *disk_cnt = 0;
            }
    }
	else
	{
		(*disk_cnt)++;

		if (*disk_cnt == CYCLE_LENGTH_OF_DISK_OPERATION) {
			IOregisters_lst[DiskStatus] = FREE;
			int disk_cmd = IOregisters_lst[DiskCmd];
			switch (disk_cmd)
			{
			case(READ):
				// copy 128 words from disk sector to memory_data[DISKBUFFER]
				memcpy(data_mem + IOregisters_lst[DiskBuffer], disk_mem + IOregisters_lst[DiskSector] * WORDS_IN_SECTOR, SECTOR_SIZE_IN_BYTES);
				break;

			case(WRITE):
				// copy 128 words from memory data[DISKBUFFER] to disk sector
				memcpy(disk_mem + IOregisters_lst[DiskSector] * WORDS_IN_SECTOR, data_mem + IOregisters_lst[DiskBuffer], SECTOR_SIZE_IN_BYTES);
				break;

			case(DO_NOTHING):
			default:
				break;
			}
			IOregisters_lst[DiskCmd] = DO_NOTHING;
			IOregisters_lst[Irq1Status] = ENABLED;
		}
	}
}

/// <summary>
/// handel monitor files
/// </summary>
/// <param name="IOregisters_lst"></param>
/// <param name="monitor_mem"></param>
void handel_monitor(DATA* IOregisters_lst, char* monitor_mem)
{

	if (IOregisters_lst[MonitorCMD])
	{
		if (IOregisters_lst[MonitorADDR] < SCREEN_X_AXIS * SCREEN_Y_AXIS)
		{
			monitor_mem[IOregisters_lst[MonitorADDR]] = IOregisters_lst[MonitorDATA];
		}
		IOregisters_lst[MonitorCMD] = 0; // handled monitor updating, toggle command bit
	}
}

/// <summary>
/// handeling interupt req
/// </summary>
/// <param name="IOregisters_lst"></param>
/// <param name="irq"></param>
/// <param name="interrupt_input"></param>
/// <param name="PC"></param>
void handel_interrupt(DATA* IOregisters_lst, BOOL *irq, BOOL* interrupt_input, unsigned short* PC)
{
	*irq = (IOregisters_lst[Irq0Enable] && IOregisters_lst[Irq0Status] ||
		IOregisters_lst[Irq1Enable] && IOregisters_lst[Irq1Status] ||
		IOregisters_lst[Irq2Enable] && IOregisters_lst[Irq2Status]);

	if (*irq)
	{
		if (!*interrupt_input)
		{
			*interrupt_input = TRUE;
			IOregisters_lst[IrqReturn] = *PC;
			*PC = IOregisters_lst[IrqHandler];
		}
		else
		{
			// do nothing, wait for in_interrupt to toggle
		}
	}
}

/// <summary>
/// handel leds file
/// </summary>
/// <param name="cur_inst"></param>
/// <param name="registers_lst"></param>
/// <param name="cycles_cnt"></param>
/// <param name="IOregisters_lst"></param>
/// <param name="leds"></param>
void handel_leds(Instruction* cur_inst,DATA* registers_lst, long long int* cycles_cnt, DATA* IOregisters_lst, FILE* leds)
{
	int reg = registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt];
	if (cur_inst->opcode == OUT && (reg == Leds))
	{
		fprintf(leds, "%ld %08x\n", *cycles_cnt, IOregisters_lst[reg]);
	}
}

/// <summary>
/// handel disply7seg
/// </summary>
/// <param name="cur_inst"></param>
/// <param name="registers_lst"></param>
/// <param name="cycles_cnt"></param>
/// <param name="IOregisters_lst"></param>
/// <param name="display7seg"></param>
void handel_disply7seg(Instruction* cur_inst, DATA* registers_lst, long long int* cycles_cnt, DATA* IOregisters_lst, FILE* display7seg)
{
	int reg = registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt];
	if (cur_inst->opcode == OUT && ( reg == Display7Seg))
	{
		fprintf(display7seg, "%ld %08x\n", *cycles_cnt, IOregisters_lst[reg]);
	}
}

/// <summary>
/// handel hw_trace if needed
/// </summary>
/// <param name="cur_inst"></param>
/// <param name="cycles_cnt"></param>
/// <param name="IOregisters_lst"></param>
/// <param name="registers_lst"></param>
/// <param name="hwregtrace"></param>
void handel_hwtrace(Instruction* cur_inst, long long int* cycles_cnt, DATA* IOregisters_lst,DATA* registers_lst, FILE* hwregtrace)
{
	if (cur_inst->opcode == OUT || cur_inst->opcode == IN)
	{
		IO_HW_Register dest_io_reg = registers_lst[cur_inst->rs] + registers_lst[cur_inst->rt];

		char str[100];
		get_string_from_io_reg(dest_io_reg, str);

		fprintf(hwregtrace, "%ld ", *cycles_cnt);
		if (cur_inst->opcode == IN)
			fprintf(hwregtrace, "%s ", "READ");
		else if (cur_inst->opcode == OUT)
			fprintf(hwregtrace, "%s ", "WRITE");
		fprintf(hwregtrace, "%s ", str);
		fprintf(hwregtrace, "%08lx\n", IOregisters_lst[dest_io_reg]);
	}
}

/// <summary>
/// loads the imutabel regs
/// </summary>
/// <param name="registers_lst"></param>
/// <param name="cur_inst"></param>
void load_const_val_to_reg_list(DATA* registers_lst, Instruction* cur_inst)
{
	registers_lst[ZERO] = 0;
	registers_lst[IMM1] = cur_inst->immediate1;
	registers_lst[IMM2] = cur_inst->immediate2;
}

/// <summary>
/// handel irq2 output
/// </summary>
/// <param name="cycles_cnt"></param>
/// <param name="irq_2_input"></param>
/// <param name="IOregisters_lst"></param>
void handel_irq2(long long int* cycles_cnt, long long int* irq_2_input, DATA* IOregisters_lst)
{
	long long c;
	while ((c = *(irq_2_input++)) != -1)
	{
		if (*cycles_cnt == c)
		{
			IOregisters_lst[Irq2Status] = ENABLED;
		}
	}
}

/// <summary>
/// increse cycle_cnt and file pounter at the end of the line
/// </summary>
/// <param name="cycles_cnt"></param>
/// <param name="leds"></param>
/// <param name="display7seg"></param>
/// <param name="trace"></param>
/// <param name="hwregtrace"></param>
void finish_up_line(long long int* cycles_cnt, FILE* leds, FILE* display7seg, FILE* trace, FILE* hwregtrace)
{
	(*cycles_cnt)++;
	fflush(leds);
	fflush(display7seg);
	fflush(trace);
	fflush(hwregtrace);
}