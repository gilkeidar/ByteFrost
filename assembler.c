#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "instruction.h"


char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];

int current_line = 1;

char *input_buf;

//Instruction instructions[] = {}

int fill_line_buffer(FILE *ifptr);

int write_machine_code();

int get_comment();

void write_line(uint8_t * instruction, FILE * ofptr);

int get_instruction(int num_tokens, uint8_t *instruction);

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
	while (fill_line_buffer(ifptr))
	{
		instruction[0] = instruction[1] = 0;
		//printf("writing machine code\n");
		num_tokens = write_machine_code();	//	Convert Assembly language line to machine language and write to output file
		//printf("Number of tokens: %d\n", num_tokens);
		
		if (!get_instruction(num_tokens, instruction))
			write_line(instruction, ofptr);

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


void write_line(uint8_t * instruction, FILE * ofptr) {

	char comment[2] = "//";
	char *current_char = input_buf;
	int has_comment = 0;

	while (isspace(*current_char))
		current_char++;
	if (*current_char == '/')
		has_comment = 1;

	if (has_comment)
		fprintf(ofptr, "%s\n",input_buf);
	else
		fprintf(ofptr, "0x%02x,  0x%02x,  %s %s\n", instruction[0], instruction[1], comment, input_buf);

}

int fill_line_buffer(FILE *ifptr)
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
            case 'h':
            case 'l':
                param_seq[i] = out_hl;
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
    if (get_param_seq(param_seq, num_tokens - 1))
        return 1;   // Error

    /*for (int j = 0; j < num_tokens - 1; j++)
        printf("%d ", param_seq[j]);
    printf("\n");*/
    //  Search for current instruction in instruction array
    int i;
    for (i = 0; i < NUM_INSTRUCTIONS; i++)
    {
        //printf("testing %s\n", tokens[0]);
        // First, search by number of parameters
        if (assembly[i].num_params == (num_tokens - 1))
        {
            //printf("Matched number of params!\n");
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
                }
            }
        }
    }

    return 0;   // (no errors)
    
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
		return atoi(&(immediate_token[1]));
	}
	
	fprintf(stderr, "Error on line %d: invalid immediate\n%s\n", current_line, input_buf);

	exit(1);
}

int get_out_hl(char * token)
{
    switch (tolower(token[0]))
    {
        case 'h':
            return 1;
        case 'l':
            return 0;
        default:
            return 0;
    }
}

int param_value(char * token, param type)
{
    switch (type)
    {
        case reg:
            return get_register(token);
        case immediate:
            return get_immediate(token);
        case out_hl:
            return get_out_hl(token);
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
        //printf("instr: 0x%04x\n", instr);
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
