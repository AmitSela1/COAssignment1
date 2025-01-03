#include "assembler_functions.h"

/// <summary>
///  reads the cuurent line from the input file
/// </summary>
/// <param name="input_file"></param>
/// <param name="line"></param>
/// <returns></returns>
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
///  loads the insruction to imemin
/// </summary>
/// <param name="line"></param>
/// <param name="inst"></param>
/// <param name="labels"></param>
/// <param name="imemin"></param>
/// <returns></returns>
int get_and_encode_instruction(const char* line, Instruction* inst, char* labels[DEPTH_OF_INSTRUCTIONS], FILE* imemin)
{
	char* field;
	char* delim = " ,";
	TYPE_OF_INSTRUCTION num = 0;
	int bit_index = SIZE_OF_INSTRUCTION_BITS;
	char leading_zeros[SIZE_OF_INSTRUCTION_CHARS + 1] = "000000000000";
	char hex[SIZE_OF_INSTRUCTION_CHARS + 1];

	// get opcode
	field = strtok(line, delim);
	inst->opcode = get_op(field);

	// get rd
	field = strtok(NULL, delim);
	inst->rd = get_reg(field);

	// get rs
	field = strtok(NULL, delim);
	inst->rs = get_reg(field);

	// get rt
	field = strtok(NULL, delim);
	inst->rt = get_reg(field);

	// get rm
	field = strtok(NULL, delim);
	inst->rm = get_reg(field);

	// get imm1
	field = strtok(NULL, delim);
	inst->immediate1 = get_num_val(field, labels);

	// get imm2
	field = strtok(NULL, delim);
	inst->immediate2 = get_num_val(field, labels);



	bit_index -= OPCODE_SIZE_IN_BITS;
	num |= ((TYPE_OF_INSTRUCTION)(inst->opcode) << bit_index);

	bit_index -= RD_SIZE_IN_BITS;
	num |= ((TYPE_OF_INSTRUCTION)(inst->rd) << bit_index);

	bit_index -= RS_SIZE_IN_BITS;
	num |= ((TYPE_OF_INSTRUCTION)(inst->rs) << bit_index);

	bit_index -= RT_SIZE_IN_BITS;
	num |= ((TYPE_OF_INSTRUCTION)(inst->rt) << bit_index);

	bit_index -= RM_SIZE_IN_BITS;
	num |= ((TYPE_OF_INSTRUCTION)(inst->rm) << bit_index);

	bit_index -= IMM1_SIZE_IN_BITS;
	num |= (((TYPE_OF_INSTRUCTION)(inst->immediate1) << bit_index) & ((TYPE_OF_INSTRUCTION)0xfff << bit_index)); 

	bit_index -= IMM2_SIZE_IN_BITS;
	num |= (((TYPE_OF_INSTRUCTION)(inst->immediate2) << bit_index) & ((TYPE_OF_INSTRUCTION)0xfff << bit_index)); 
	sprintf(hex, "%llX", num);
	int to_pad = SIZE_OF_INSTRUCTION_CHARS - strlen(hex);

	if (to_pad)
	{
		memcpy(leading_zeros + to_pad, hex, SIZE_OF_INSTRUCTION_CHARS - to_pad);
		fwrite(leading_zeros, 1, SIZE_OF_INSTRUCTION_CHARS, imemin);
	}
	else
		fwrite(hex, 1, SIZE_OF_INSTRUCTION_CHARS, imemin);

	fwrite("\n", 1, strlen("\n"), imemin);

	return 0;
}

/// <summary>
/// used to sort and deal with the different line type
/// </summary>
/// <param name="cur_line"></param>
/// <param name="sInstruction"></param>
/// <param name="label_addresses_lst"></param>
/// <param name="fImemin"></param>
/// <returns></returns>
Line_Type get_line_type(char* cur_line, Instruction* sInstruction ,char* label_addresses_lst[DEPTH_OF_INSTRUCTIONS], FILE* fImemin){
	Line_Type return_value = REGULAR_INST;
	char cur_cleaned_line[MAX_LINE_LENGTH];
	int cleaned_length = clean_line(cur_line, cur_cleaned_line);

	if (cleaned_length==0) {
		//This is comment, do nothing
		return_value = COMMENT;
	}
	//checks if the line conatains label or label + instruction
	else if (found_label(cur_cleaned_line)) {  // check if the line contains a label
		strtok(cur_cleaned_line, ":");
		char* maybe_inst = strtok(NULL, ":");
		if (maybe_inst)
			return_value = get_line_type(maybe_inst, sInstruction, label_addresses_lst, fImemin);
		else
			return_value = LABEL;
	}
	else if (line_is_comment(cur_cleaned_line)) {
		return_value = COMMENT;
	}
	else if (found_pseudo(cur_cleaned_line)) {

		return_value = PSEUDO;
	}
	else {
		// manage actual instruction
		get_and_encode_instruction(cur_cleaned_line, sInstruction, label_addresses_lst, fImemin);
		return_value = REGULAR_INST;
	}

	return return_value;
}

/// <summary>
/// fixes trailing, leading, inbetweening whitespaces and tabs in given string
/// </summary>
/// <param name="line_to_fix"></param>
/// <param name="line_fixed"></param>
/// <returns></returns>
int clean_line(const char* line_to_fix, char* line_fixed)
{
	//variable declaration
	char c;
	int i = 0;
	int j = 0;
	int spaced = 0;
	int first = 1;
	char temp[MAX_LINE_LENGTH];
	temp[MAX_LINE_LENGTH - 1] = '\0';
	char* ptr;

	while ((c = line_to_fix[i]) != '\0') {
		if (spaced)
		{
			if (c == '\t' || c == ' ') {}
			else
			{
				// exited from space state
				spaced = 0;
			}
		}

		if (!spaced)
		{
			if (c == '\t' || c == ' ')
			{
				if (first)
				{
					i++;
					continue;
				}
				else
					line_fixed[j] = ' ';
				spaced = 1;
			}
			else
			{
				first = 0;
				line_fixed[j] = c;
			}
			j++;
		}
		i++;
	}

	// remove trailing spaces:
	if (line_fixed[j - 1] == ' ')
		line_fixed[j - 1] = '\0';
	else
		line_fixed[j] = '\0';

	while ((ptr = strstr(line_fixed, " ,")) != NULL)
	{
		int len = strlen(line_fixed);
		strncpy(temp, line_fixed, ptr - line_fixed);
		strncpy(temp + (ptr - line_fixed), ptr + 1, len - (ptr - line_fixed) + 1);
		temp[len - 1] = '\0';
		strncpy(line_fixed, temp, strlen(temp));
		line_fixed[len - 1] = '\0';
	}

	return j;
}

/// <summary>
/// returns an enum value of an opcode according to string
/// </summary>
/// <param name="opcode"></param>
/// <returns></returns>
OpCode get_op(const char* opcode)
{
	// List of all opcodes as strings in correspondence with the updated OpCode enum order
	static const char* opcodes_str[] = {
		"add", "sub", "mac", "and", "or", "xor",
		"sll", "sra", "srl", "beq", "bne", "blt",
		"bgt", "ble", "bge", "jal", "lw", "sw",
		"reti", "in", "out", "halt", NULL
	};
	lower_str(opcode, opcode);
	// Iterate through the opcodes_str array
	for (int i = 0; opcodes_str[i] != NULL; i++) {
		// Compare the input opcode string with each opcode string in the array
		if (strcmp(opcode, opcodes_str[i]) == 0) {
			// If a match is found, return the corresponding OpCode enum value
			return (OpCode)i;
		}
	}
}

/// <summary>
/// returns an enum value of a register according to string
/// </summary>
/// <param name="reg"></param>
/// <returns></returns>
Register get_reg(const char* reg)
{
	//char lower_reg[MAX_REG_NAME]; // Assume MAX_REG_NAME is defined appropriately
	lower_str(reg, reg); // Converts 'reg' to lowercase and stores in 'lower_reg'

	// Array of register names in lowercase to match the lowercase input
	const char* reg_names_str[] = {
		"$zero", "$imm1", "$imm2", "$v0", "$a0", "$a1", "$a2", "$t0",
		"$t1", "$t2", "$s0", "$s1", "$s2", "$gp", "$sp","$ra"
	};

	for (int i = 0; i < 16; i++) {
		if (strcmp(reg, reg_names_str[i]) == 0) {
			return (Register)i;
		}
	}
}

/// <summary>
/// changing every ascii char to lowercase
/// </summary>
/// <param name="input"></param>
/// <param name="output"></param>
void lower_str(const char* input, char* output) {
	// Ensure input is not NULL
	if (!input) {
		output[0] = '\0'; // Make output an empty string if input is NULL
		return;
	}
	int i;
	for (i = 0; input[i] != '\0'; ++i) {
		output[i] = tolower((unsigned char)input[i]); // Cast to unsigned char to handle negative chars
	}
	output[i] = '\0'; // Null-terminate the output string
}

/// <summary>
///  returns a numeric value inside field, using labels as a map to labels indices
/// </summary>
/// <param name="field"></param>
/// <param name="label_addresses"></param>
/// <returns></returns>
int get_num_val(const char* field, char* label_addresses[DEPTH_OF_INSTRUCTIONS]) {

	char temp[MAX_LINE_LENGTH] = { 0 };
	lower_str(field, temp); // ensure input is lowercase
	
	// hexadecimal handling
	if (temp[0] == '0' && (temp[1] == 'x' || temp[1] == 'X')) {
		unsigned int value;
		sscanf(temp + 2, "%x", &value);
		return (int)value;
	}

	// label handling
	if (isalpha((unsigned char)temp[0])) {
		for (int i = 0; i < DEPTH_OF_INSTRUCTIONS; i++) {
			if (label_addresses[i] && strcmp(label_addresses[i], temp) == 0) {
				return i; // Return the index for the label
			}
		}
	}

	// decimal handling
	return atoi(temp);

}

/// <summary>
/// returns a sting cointains the label if contains label or null if doesn't 
/// </summary>
/// <param name="cur_cleaned_line"></param>
/// <param name="label_and_inst"></param>
/// <returns></returns>
char* get_label(char* cur_cleaned_line, BOOL* label_and_inst)
{
	int length_of_label = 0;
	char* str_Label = NULL;
	clean_line(cur_cleaned_line, cur_cleaned_line);
	if (found_label(cur_cleaned_line)) {
		char* T = strtok(cur_cleaned_line, ":");
		length_of_label = strlen(T);
		str_Label = malloc((length_of_label + 1) * sizeof(char));
		strncpy(str_Label, T, length_of_label + 1);
		lower_str(str_Label, str_Label);

		//checks of the label was inline label and there is an istruction after
		T = strtok(NULL, ":");
		if (T) {
			clean_line(T, cur_cleaned_line);
			if (!line_is_comment(cur_cleaned_line))
				*label_and_inst = TRUE;
		}
	}
	return str_Label;
}

/// <summary>
/// loads the data to the memory from the pseudo instaction
/// </summary>
/// <param name="cur_line"></param>
/// <param name="memory_data"></param>
void manage_pseudo(const char* cur_line, int* memory_data) {
	char* field;
	char* space = " ";
	Word pseudo;

	// skip instruction (known to be .word at this point)
	field = strtok(cur_line, space);

	field = strtok(NULL, space);
	pseudo.address = get_num_val(field, NULL);

	field = strtok(NULL, space);
	pseudo.value = get_num_val(field, NULL);

	memory_data[pseudo.address] = pseudo.value;
}

/// <summary>
/// dumps the memory data to a file in the specified format
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
/// finds the last non-zero address in memory
/// </summary>
/// <param name="data_memory"></param>
/// <param name="memory_size"></param>
/// <returns></returns>
int find_last_non_zero(DATA* data_memory, int memory_size)
{
	int idx_of_last_non_zero = 0;
	for (int i = 0; i < memory_size; i++)
	{
		if (data_memory[i] != 0)
		{
			idx_of_last_non_zero = i;
		}
	}
	return idx_of_last_non_zero;
}

/// <summary>
/// return TRUE if the current line is indeed a labellabel
/// </summary>
/// <param name="cur_line"></param>
/// <returns></returns>
BOOL found_label(const char* cur_line) {
	for (int i = 0; cur_line[i] != '\0'; ++i) {
		if (cur_line[i] == '#') {
			return FALSE; // If we hit a comment before finding a label, return FALSE.
		}
		if (cur_line[i] == ':') {
			return TRUE; // If we find a colon, it indicates a label.
		}
	}
	return FALSE; // Return FALSE if no label is found.
}

/// <summary>
/// returns TRUE if given line is a pseudo instruction
/// </summary>
/// <param name="cur_line"></param>
/// <returns></returns>
BOOL found_pseudo(const char* cur_line) {
	for (int i = 0; cur_line[i] != '\0'; ++i) {
		if (cur_line[i] == '#') {
			return FALSE; // If we hit a comment before finding a label, return FALSE.
		}
		if (cur_line[i] == '.') {
			return TRUE; // If we find a colon, it indicates a label.
		}
	}
	return FALSE; // Return FALSE if no label is found.
}

/// <summary>
/// returns TRUE if the whole line is a comment
/// </summary>
/// <param name="cleared_line"></param>
/// <returns></returns>
BOOL line_is_comment(const char* cleared_line) {
	return ((cleared_line[0] == '#') || (strlen(cleared_line) == 0));
}


