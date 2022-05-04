#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#define MAX_LINE_LENGTH	300

char tokens[4][20];

int current_line = 1;

char *input_buf;

int fill_line_buffer(char * input_buf, FILE *ifptr);

int write_machine_code(char *input_buf);

int get_instruction(int num_tokens, uint8_t *instructions);

void write_line(uint8_t * instruction, char * input_buf, FILE * ofptr);

int main (int argc, char * args [])
{
	if (argc == 1)
	{
		printf("Usage: ./assembler [assembly file name]\n");
		return 1;
	}

	char *input_file = (char *)malloc((strlen(args[1]) + 1) * sizeof(char));
	strcpy(input_file, args[1]);
	char *output_file = (char *)malloc((strlen(input_file) + 1) + 2 * sizeof(char));
	strcpy(output_file, input_file);
	output_file = strcat(output_file, ".a");

	FILE *ifptr = fopen(input_file, "r");	//	Attempt to open given file
	FILE *ofptr;

	if (ifptr == NULL)
	{
		printf("Error: file %s does not exist\n", input_file);
		return 2;
	}

	// Successfully opened input file
	//printf("successfully opened input file\n");

	ofptr = fopen(output_file, "w");	//	Create output file

	//	Loop to read through file line-by-line
	input_buf = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));	//	Input buffer (for each line in text file)
	int num_tokens;
	uint8_t instruction[2];
	while (fill_line_buffer(input_buf, ifptr))
	{
		instruction[0] = instruction[1] = 0;
		//printf("writing machine code\n");
		num_tokens = write_machine_code(input_buf);	//	Convert Assembly language line to machine language and write to output file
		//printf("Number of tokens: %d\n", num_tokens);
		
		if (get_instruction(num_tokens, instruction))
			write_line(instruction, input_buf, ofptr);

		current_line++;
	}

	//printf("exited loop\n");

	free(input_file);
	free(output_file);
	free(input_buf);
	
	fclose(ifptr);
	fclose(ofptr);

	return 0;


}

void write_line(uint8_t * instruction, char * input_buf, FILE * ofptr) {

	char comment[2] = "//";
	char *current_char = input_buf;

	while (isspace(*current_char))
		current_char++;
	if (*current_char == '/')
		comment[0] = '\0';

	fprintf(ofptr, "0x%02x,  0x%02x,  %s %s\n", instruction[0], instruction[1], comment, input_buf);
}

int get_register(char * register_token)
{
	if (!strcmp(register_token, "R0"))
		return 0;

	if (!strcmp(register_token, "R1"))
		return 1;

	if (!strcmp(register_token, "R2"))
		return 2;
	
	if (!strcmp(register_token, "R3"))
		return 3;

	return -1;
}

int get_immediate(char * immediate_token)
{
	if (immediate_token[0] == '#')
	{
		return atoi(&(immediate_token[1]));
	}
	
	fprintf(stderr, "Error on line %d: invalid immediate\n%s\n", current_line, input_buf);

	exit(1);
}


int get_ALU(int num_tokens, uint8_t * instruction, int ALU_type, int opcode) {
	// If ALU_type == 0 --> ALU register, ALU_type == 1 --> ALU immediate
	instruction[0] = (get_register(tokens[1]) << 6) | ( (ALU_type == 0) ? 2 : 6);

	if (!ALU_type)
	{
		// ALU register
		instruction[1] = (get_register(tokens[3]) << 6) | (get_register(tokens[2]) << 4) | opcode; 
	}

	return 1;
}

int get_OUT(int num_tokens, uint8_t * instruction) {
	int low_bit = 1;	// By default write to low bit
	int is_data = 0;	// By default assume to print data

	if (num_tokens >= 3)
	{
		switch(tolower(tokens[2][0]))
		{
			case 'l':
				low_bit = 1;
				break;
			case 'h':
				low_bit = 0;
				break;
		}
	} 
	if (num_tokens >= 4)
	{
		switch(tolower(tokens[3][0]))
		{
			case 'i':
				is_data = 0;
				break;
			case 'd':
				is_data = 1;
				break;
		}
	}

	instruction[0] = (is_data << 5) | (low_bit << 4) | 8;
	instruction[1] = get_register(tokens[1]) << 4;

	return 1;
}

int get_LDR(int num_tokens, uint8_t * instruction) {
	if (num_tokens != 3)
	{
		fprintf(stderr, "Error: Incorrect number of parameters for %s instruction in line %d\n%s\n", tokens[0], current_line, input_buf);
		return 0;	// Error - too few parameters for LDR
	}
	instruction[0] = (get_register(tokens[1]) << 6) | 3;
	instruction[1] = get_immediate(tokens[2]);

	return 1;
}

int get_branch(int num_tokens, uint8_t * instruction) {
	if (num_tokens != 2)
	{
		fprintf(stderr, "Error: Incorrect number of parameters for %s instruction in line %d\n%s\n", tokens[0], current_line, input_buf);
		return 0;	// Error - incorrect number of parameters for branch	
	}

	int branch_type;
	if (!strcmp(tokens[0], "JMP"))
		branch_type = 0;
	else if (!strcmp(tokens[0], "BIN"))
		branch_type = 1;
	else if (!strcmp(tokens[0], "BIC"))
		branch_type = 2;
	else if (!strcmp(tokens[0], "BEQ"))
		branch_type = 3;
	else if (!strcmp(tokens[0], "BPL"))
		branch_type = 5;
	else if (!strcmp(tokens[0], "BNC"))
		branch_type = 6;
	else if (!strcmp(tokens[0], "BNE"))
		branch_type = 7;
	else
		branch_type = 4;	// Never branch (NOP)
	
	instruction[0] = (branch_type << 5) | 5;
	instruction[1] = get_immediate(tokens[1]) - 1;	// Subtract 1 from immediate since in text editors, first line is line 1, but would be 0 in machine lang

	return 1;

}

int get_BRK(int num_tokens, uint8_t * instruction) {
	if (num_tokens != 1)
	{
		fprintf(stderr, "Error: Incorrect number of parameters for %s instruction in line %d\n%s\n", tokens[0], current_line, input_buf);
		return 0;
	}

	instruction[0] = 1;
	instruction[1] = 0;
	
	return 1;
}

int get_instruction(int num_tokens, uint8_t * instruction) {
	if (!num_tokens)
		return 0;
	
	switch (num_tokens)
	{
		case 1:	// NOP, BRK
			if (!strcmp(tokens[0], "BRK"))
				return get_BRK(num_tokens, instruction);
			fprintf(stderr, "Error: Unknown instruction \"%s\" in line %d:\n%s\n", tokens[0], current_line, input_buf);
			break;
		case 2:	// OUT
			if (!strcmp(tokens[0], "OUT"))
				return get_OUT(num_tokens, instruction);
			if (!strcmp(tokens[0], "JMP") || !strcmp(tokens[0], "BIN") 
				|| !strcmp(tokens[0], "BIC") || !strcmp(tokens[0], "BEQ")
				|| !strcmp(tokens[0], "BPL") || !strcmp(tokens[0], "BPL")
				|| !strcmp(tokens[0], "BNC") || !strcmp(tokens[0], "BNE"))
				return get_branch(num_tokens, instruction); 
			fprintf(stderr, "Error: Unknown instruction \"%s\" in line %d:\n%s\n", tokens[0], current_line, input_buf);
			break;
		case 3:	// ALU Immediate
			if (!strcmp(tokens[0], "OUT"))
				return get_OUT(num_tokens, instruction);
			if (!strcmp(tokens[0], "LDR"))
				return get_LDR(num_tokens, instruction);

			
			fprintf(stderr, "Error: Unknown instruction \"%s\" in line %d:\n%s\n", tokens[0], current_line, input_buf);
			break;
		case 4:	// ALU Registers
			if (!strcmp(tokens[0], "ADD"))
				return get_ALU(num_tokens, instruction , 0 , 6);
			if (!strcmp(tokens[0], "OUT"))
				return get_OUT(num_tokens, instruction);

			fprintf(stderr, "Error: Unknown instruction \"%s\" in line %d:\n%s\n", tokens[0], current_line, input_buf);
			break;
		default:
			fprintf(stderr, "Error: Unexpected number of tokens in line %d:\n%s\n", current_line, input_buf);
	}

	//printf("Instruction: %#x, %#x\n", instruction[0], instruction[1]);

	return 0;
}

int fill_line_buffer(char *input_buf, FILE *ifptr)
{
	char curr_char;
	int i = 0;

	//	Fill input_buf
	while ( (curr_char = getc(ifptr)) != EOF || i < MAX_LINE_LENGTH)
	{
		input_buf[i] = curr_char;
		
		if (curr_char == '\n')	//	If new line, end input buffer string
		{
			input_buf[i] = '\0';
			break;
		}
		i++;
	}

	if (curr_char != '\n')	//	If line is longer than buffer, cut it off (must be a comment since no assembly instruction is this long)
	{
		input_buf[i - 1] = '\0';
	}
	//printf("Current line contents: %s\n", input_buf);


	return (curr_char != EOF);
}


int get_comment(char *input_buf) {
	char * current_char = input_buf;
	while (*current_char != '\0')
	{
		if (*current_char == '/')
		{
			if (*(current_char + 1) == '/')
			{
				break;
			}
		}

		current_char++;
	}
	return current_char - input_buf;
}


int write_machine_code(char *input_buf)
{
	int comment_index = get_comment(input_buf), i;

	int curr_token = 0;
	int curr_char = 0;

	int white_space = 1;

	//printf("Current line: %s\n", input_buf);

	for (i = 0; i < comment_index; i++) 
	{
		if (isspace(input_buf[i]) || input_buf[i] == ',')
		{
			if (!white_space)
			{
				tokens[curr_token][curr_char] = '\0';
				curr_token++;
				curr_char = 0;
			}
			if (curr_token >= 4)
			{
				break;
			}
			white_space = 1;
			continue;
		}
		white_space = 0;
				
		tokens[curr_token][curr_char] = input_buf[i];

		curr_char++;
	}


	if (!white_space)
	{
		tokens[curr_token][curr_char] = '\0';
		curr_token++;
	}

	for (i = 0; i < curr_token; i++)
	{
		//printf("Token:\t%s\n", tokens[i]);

	}

	return curr_token;
}
