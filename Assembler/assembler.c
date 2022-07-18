#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "instruction.h"


char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];	// Assembly code split into tokens

int current_line = 1;						// Current line of assembly file

int current_instruction = 1;				// Current instruction (not necessarily line)

char *input_buf;							// Input buffer (stores current line)

int finished_reading = 0;					// Finished reading file

int fill_line_buffer(FILE *ifptr);

int write_machine_code();

int get_comment();

void write_line(uint8_t * instruction, FILE * ofptr, int num_tokens, int binary_flag);

int get_instruction(int num_tokens, uint8_t *instruction);

int verify_file_extension(char *filename, char *extension);

char * get_output_file(char *input_file, int binary_flag);

int main (int argc, char * args [])
{
	// If no input parameters, print usage text
	if (argc == 1)
	{
		printf("Usage: ./assembler assembly_file [-b]\n");
		return 1;
	}

	// Handle flags
	int binary_flag = 0;	// If invoked, compile to binary file (instead of string array format)
	int i;
	int found_input = 0;
	char *input_file;
	for (i = 1; i < argc; i++)
	{
		if (args[i][0] == '-')
		{
			// Beginning of flag

			// If no flag specified (i.e. just saw '-'), error
			if (args[i][1] == '\0')
			{
				fprintf(stderr, "Error - empty flag argument.\n");
				exit(1);
			}
			// Otherwise, handle flag
			switch (args[i][1])
			{
				case 'b':
					binary_flag = 1;
					break;
				default:
					fprintf(stderr, "Error: Unknown flag %s\n", args[i]);
			}
		}
		else if (!found_input)
		{
			input_file = (char *)malloc((strlen(args[i]) + 1) * sizeof(char));
			strcpy(input_file, args[1]);
			found_input = 1;
		} 
	}

	if (!found_input)
	{
		fprintf(stderr, "Error - no input file given.\n");
		exit(1);
	}

	// Found input file and handled flag

	// Check that input file has correct .asm ending
	if (!verify_file_extension(input_file, "asm"))
	{
		// Error - wrong file extension
		fprintf(stderr, "Error: Incorrect file extension (input files must be .asm files)\n");
		free(input_file);
		exit(1);
	}

	// Get output file nname
	char *output_file = get_output_file(input_file, binary_flag);

	/*char *output_file = (char *)malloc((strlen(input_file) + 1) + 4 * sizeof(char));
	strcpy(output_file, input_file);

	if (!binary_flag)
		output_file = strcat(output_file, ".a");
	else
		output_file = strcat(output_file, ".bin");*/

	FILE *ifptr = fopen(input_file, "r");	// Attempt to open given file
	FILE *ofptr;

	if (ifptr == NULL)	// Failed opening input file
	{
		fprintf(stderr, "Error: file %s does not exist\n", input_file);
		return 2;
	}

	// Successfully opened input file
	//printf("successfully opened input file\n");

	// Check binary flag (if binary, need to write bit-by-bit)
	if (binary_flag)
	{
		ofptr = fopen(output_file, "wb");	
	}
	else 
		ofptr = fopen(output_file, "w");	//	Create output file

	//	Loop to read through file line-by-line
	input_buf = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));	//	Input buffer (for each line in text file)
	int num_tokens;
	//int line_counter = 0;
	int error;
	uint8_t instruction[2];
	while (!finished_reading)
	//while (fgets(input_buf, MAX_LINE_LENGTH, ifptr) != NULL)
	{
		finished_reading = fill_line_buffer(ifptr);
		instruction[0] = instruction[1] = 0;
		//printf("writing machine code\n");
		num_tokens = write_machine_code();	//	Convert Assembly language line to machine language and write to output file
		//printf("Number of tokens: %d\n", num_tokens);
		//line_counter++;
		if (!(error = get_instruction(num_tokens, instruction)))
		{
			write_line(instruction, ofptr, num_tokens, binary_flag);
		}
		else
		{
			//fprintf(stderr, "\nError %d in line %d: Tokens %d >>>> %s\n\n", error, line_counter, num_tokens, input_buf);
			fprintf(stderr, "\nError %d in line %d: Tokens %d >>>> %s\n\n", error, current_line, num_tokens, input_buf);
 			exit(0);
		}

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

char * get_output_file(char *input_file, int binary_flag)
{
	int length = 0;
	int extensionLength;
	char *current_char = input_file;
	char *extension;
	int i;

	if (binary_flag)	// Use binary extension
		extension = ".bin";
	else				// Use machine-language extension
		extension = ".mlg";

	extensionLength = strlen(extension);

	// Count and skip over everything before the period
	while (*current_char != '.' && *current_char != '\0')
	{
		current_char++;
		length++;
	}

	// Allocate output file name
	char * output = (char *)malloc((length + extensionLength + 1) * sizeof(char));

	// Copy file name
	for (i = 0; i < length; i++)
	{
		output[i] = input_file[i];
	}

	// Copy extension
	strcpy(output + length, extension);

	// Add 0 sentinel
	output[length + extensionLength] = '\0';

	//printf("output file name: %s\n", output);

	return output;
}

int verify_file_extension(char *filename, char *extension)
{
	//printf("Verifying extension...\n");
	//printf("filename: %s\n", filename);
	// Skip until arive at '.' or end of filename
	while (*filename != '.' && *filename != '\0')
		filename++;
	filename++;		// Skip '.'
	//printf("filename: %s\n", filename);
	
	if (*filename == '\0')	// No file extension found
		return 0;
	
	// Found file extension, compare to given extension
	if (!strcmp(filename, extension))	// Extension matches given extension
		return 1;

	return 0;							// Extensions don't match
}


void write_line(uint8_t * instruction, FILE * ofptr, int num_tokens, int binary_flag) {

	// char comment[2] = "//";
	// char *current_char = input_buf;
	// int has_comment = 0;

	// while (isspace(*current_char))
	// 	current_char++;
	// if (*current_char == '/')
	// 	has_comment = 1;

	if (num_tokens==0 && !binary_flag)
	{
		fprintf(ofptr, "%s\n",input_buf);
	}
	else if (num_tokens != 0 && binary_flag)
	{
		int i;
		for (i = 0; i < 2; i++)
			fwrite(instruction + i, 1, 1, ofptr);
		/*int i, j;
		for (i = 0; i < 2; i++)
		{
			for (j = 0; j < 8; j++)
			{
				//fprintf(ofptr, "%d", instruction[i] & 1);
				//instruction[i] = instruction[i] >> 1;
				//fwrite(&(instruction[i] >> j), )
			}
		}*/
		current_instruction++;
	}
	else if (!binary_flag)
	{
		fprintf(ofptr, "0x%02x,  0x%02x, // 0x%02x: %s\n", instruction[0], instruction[1], current_instruction, input_buf);
		current_instruction++;
		//printf("0x%02x,  0x%02x, // %s\n", instruction[0], instruction[1], input_buf);
	}

	
}

int fill_line_buffer(FILE *ifptr)
{
	char curr_char;
	int i = 0;

	//	Fill input_buf
	while ( (curr_char = getc(ifptr)) != EOF || i < MAX_LINE_LENGTH)
	{
		input_buf[i] = curr_char;
		
		if (curr_char == '\n' || curr_char == 13 || curr_char == 10 || curr_char == EOF)	//	If new line, end input buffer string
		{
			input_buf[i] = '\0';
			break;
		}
		i++;
	}

	if (curr_char != '\n' && curr_char != 13 && curr_char != 10 || curr_char == EOF)	//	If line is longer than buffer, cut it off (must be a comment since no assembly instruction is this long)
	{
		input_buf[i - 1] = '\0';
	}
	//printf("Current line contents: %sbanana\n", input_buf);

	return (curr_char == EOF); // returns 0 if not done, and 1 if read last line
}

int get_comment() {
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


int write_machine_code()
{
	int comment_index = get_comment(), i;
	int curr_token = 0;
	int curr_char = 0;
	int white_space = 1;

	//printf("Current line: %s\n", input_buf);

	//printf("Comment index: %d\n", comment_index);

	for (i = 0; i < comment_index; i++) 
	{
		//printf("Current char: %c\n", input_buf[i]);
		if (isspace(input_buf[i]) || input_buf[i] == ',')
		{
			//if (!white_space && !isspace(input_buf[i + 1]) && !(input_buf[i+ 1] == ',') && !(input_buf[i + 1] == '\0'))
			if (!white_space)
			{
				tokens[curr_token][curr_char] = '\0';
				curr_token++;
				curr_char = 0;
			}
			white_space = 1;
			if (curr_token >= MAX_TOKENS)
			{
				break;
			}
			continue;
		}
		else {
			white_space = 0;
		}
		tokens[curr_token][curr_char] = input_buf[i];
		curr_char++;
	}

	if (!white_space)
	{
		//printf("Got here\n");
		tokens[curr_token][curr_char] = '\0';
		curr_token++;
	}
	for (i = 0; i < curr_token; i++)
	{
		//printf("Token:\t%s\n", tokens[i]);
	}
	return curr_token;
}

int get_param_seq(param *param_seq, int num_params)
{
    if (!num_params)
        return 0;

    int i;
    char first_char;
    for (i = 0; i < num_params; i++)
    {
        // Identify token types
        first_char = tolower(tokens[i + 1][0]);

        //printf("token[%d]: %s\n", i + 1, tokens[i + 1]);

        switch (first_char)
        {
            case 'r':
                param_seq[i] = reg;
                break;
            case '#':
                param_seq[i] = immediate;
                break;
            case 'a':
            case 'i':
                param_seq[i] = out_ai;
                break;
			case '+':
			case '-':
				param_seq[i] = branch_amt;
				break;
            default:
                param_seq[i] = instruction;
                break;
        }

        //printf("decided on %d\n", (int)param_seq[i]);
    }

    return 0;
}

int equal_sequences(param * first, param * second, int num_params) {
    int i;
    for (i = 0; i < num_params; i++)
    {
        //printf("first[%d] = %d\tsecond[%d] = %d\n", i, first[i], i, second[i]);
        if (first[i] != second[i])
            return 1;
    }

    return 0;
}

int get_instruction(int num_tokens, uint8_t *instruction)
{
    param param_seq[MAX_TOKENS];
	int instruction_found=-1;
	
	if(num_tokens==0) return 0;

    if (get_param_seq(param_seq, num_tokens - 1))
        return 1;   // Error

    // for (int j = 0; j < num_tokens - 1; j++)
    //    printf("%d ", param_seq[j]);
    //  printf("\n");
    //  Search for current instruction in instruction array
    int i;
    for (i = 0; i < NUM_INSTRUCTIONS; i++)
    {
        //printf("testing %s\n", tokens[0]);
        // First, search by number of parameters
        if (assembly[i].num_params == (num_tokens - 1))
        {
             //printf("%s: Matched number of params!\n", assembly[i].name);
            // Second, search by name
            if (!strcmp(assembly[i].name, tokens[0]))
            {
                //printf("Matched name!\n");
                // Third, search by parameter sequence
                if (!equal_sequences(assembly[i].param_seq, param_seq, num_tokens - 1))
                {
                    //printf("found instruction %s!\n", assembly[i].name);
                    // Run handler
                    assembly[i].handler(&(assembly[i]), instruction);
					instruction_found=0;
					return 0;
                }
				// printf("Name and num param match but equal_sequences failed!\n");

            }
        }
    }
	
    return instruction_found;    
    
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

	fprintf(stderr, "Error on line %d: invalid register\n%s\n", current_line, input_buf);

    exit(1);
}

int get_immediate(char * immediate_token)
{
	if (immediate_token[0] == '#')
	{
		if(immediate_token[1]=='0' && immediate_token[2]=='x')
		{
             return strtol(&(immediate_token[3]), NULL, 16);
		}

		return atoi(&(immediate_token[1]));
	}
	
	fprintf(stderr, "Error on line %d: invalid immediate\n%s\n", current_line, input_buf);

	exit(1);
}

int get_out_ai(char * token)
{
    switch (tolower(token[0]))
    {
        case 'i':
            return 1;
        case 'a':
            return 0;
        default:
            return 0;
    }
}

int get_branch_amt(char *token)
{
	char value;
	if (token[0] == '+' || token[0] == '-')
	{
		if (token[1] == '0' && token[2] == 'x')
		{
			value = strtol(&(token[3]), NULL, 16);
		}
		else
			value = atoi(&(token[1]));
		
		if (token[0] == '-')
			value = -value;
		
		return value;
	}

	fprintf(stderr, "Error on line %d: invalid branch amount\n%s\n", current_line, input_buf);

	exit(1);
}

int param_value(char * token, param type)
{
    switch (type)
    {
        case reg:
            return get_register(token);
        case immediate:
            return get_immediate(token);
        case out_ai:
            return get_out_ai(token);
		case branch_amt:
			return get_branch_amt(token);
    }
}

void set_instruction(uint16_t instr, uint8_t * instruction)
{
    instruction[0] = (uint8_t) instr;
    instruction[1] = (uint8_t) (instr >> 8);
}

/*
 *
 *  HANDLERS
 *
 */

int basic_handler(Instruction * this, uint8_t * instruction)
{
    // Get skeleton
    uint16_t instr = (this->skeleton[1] << 8) | this->skeleton[0];

    int i;
    for (i = 0; i < this->num_params; i++)
    {
       // printf("instr: 0x%04x\n", instr);
        //printf("param_value: %d\n", param_value(tokens[i + 1], this->param_seq[i]));
        instr |= param_value(tokens[i + 1], this->param_seq[i]) << (this->param_position[i]);
        
    }

    //printf("instr: 0x%04x\n", instr);

    set_instruction(instr, instruction);

    return 0;
}

int abs_branch_handler(Instruction * this, uint8_t * instruction)
{
    // Get skeleton
    uint16_t instr = (this->skeleton[1] << 8) | this->skeleton[0];

    int i;
    for (i = 0; i < this->num_params; i++)
    {
        //printf("instr: 0x%04x\n", instr);
        //printf("param_value: %d\n", param_value(tokens[i + 1], this->param_seq[i]));
        int value = param_value(tokens[i + 1], this->param_seq[i]);

        if (this->param_seq[i] == immediate)    // Subtract 1 from immediate for branch handler
            value--;
        instr |=  value << (this->param_position[i]);
        
    }

    //printf("instr: 0x%04x\n", instr);

    set_instruction(instr, instruction);

    return 0;
}

int rel_branch_handler(Instruction * this, uint8_t * instruction)
{
	// Get skeleton
	uint16_t instr = (this->skeleton[1] << 8) | this->skeleton[0];

	int i;
	for (i = 0; i < this->num_params; i++)
	{
		char value = param_value(tokens[i + 1], this->param_seq[i]);
		//printf("value: %d\n", value);
			// Subtract 1 from the immediate for the relative branch handle (i.e. JMP +0 will stay on this line,
					// but this needs to cancel the PC increase so it will actually do JMP -1 to stay in place)
		value--;
		//printf("value: %d\n", value);

		/*if (value < 0)
			value = 256 + value;*/

		//printf("value: %x\n", value);
		instr |= value << (this->param_position[i]);
	}

	set_instruction(instr, instruction);

	return 0;
}
