#include <stdlib.h>
#include <stdio.h>
// This is unix only #include <unistd.h> - source code for getopt is added 
#include <string.h>
#include <ctype.h>
#include "errorv2.h"
#include "label.h"
#include "instruction.h"
int verify_file_extension(char *filename, char *extension);
void get_input(char ** argv, int argc, int * b_flag, char ** input_file_name, char ** output_file_name);
char * get_output_file(char *input_file, int binary_flag);
int fill_line_buffer(FILE *ifptr);
int write_machine_code();
int get_comment();
void write_line(uint8_t * instruction, FILE * ofptr, int num_tokens, int binary_flag);
int get_instruction(int num_tokens, uint8_t *instruction);
char * get_label_name(char * label);

void traverse_label_tree(Label * current);

int current_line = 1;                       //  Current line of assembly file
int current_instruction = 1;                //  Current instruction (not necessarily line)
char *input_buf;                            //  Input buffer (stores current line)
int finished_reading = 0;                   //  Finished reading file
char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];	//  Assembly code split into tokens
Label * root = NULL;                        //  Label binary search tree root

int main(int argc, char ** argv)
{
    /*  ByteFrost Assembler */

    /*  Step 1: Read flags and input file name  */
    char * input_file_name;     //  Input file name
    char * output_file_name;    //  Output file name
    int b_flag = 0;             //  Binary flag (convert .asm file to .bin file)

    //  Get inputs
    get_input(argv, argc, &b_flag, &input_file_name, &output_file_name);

    //  Got input
    printf("Input file name:\t%s\n", input_file_name);
    printf("Output file name:\t%s\n", output_file_name);

    /*  Step 2: Run through input file twice to get and replace labels with addresses   */
    FILE *ifptr = fopen(input_file_name, "r");
    FILE *ofptr;

    //  Open output file
    if (b_flag)
    {
        ofptr = fopen(output_file_name, "wb");
    }
    else
    {
        ofptr = fopen(output_file_name, "w");
    }

    //  If ifptr is null, then the input file doesn't exist
    if (ifptr == NULL)
    {
        fprintf(stderr, "ERROR: INPUT_FILE_DOES_NOT_EXIST: File %s not found. Are you in the right directory?\n", input_file_name);
        exit(INPUT_FILE_DOES_NOT_EXIST);
    }

    //  Allocate memory for input buffer
    input_buf = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));

    int i;
    int num_tokens;
    uint8_t instruction[2];
    
    for (int i = 0; i < 2; i++)
    {
        if (i == 1)
        {
            printf("\n\nSECOND PASS\n\n");
        }
        while (!finished_reading)
        {
            //  Get current line
            finished_reading = fill_line_buffer(ifptr);
            instruction[0] = instruction[1] = 0;

            //  Get number of tokens in current line + fill tokens array
            num_tokens = write_machine_code();

            if (!get_instruction(num_tokens, instruction))
            {
                write_line(instruction, ofptr, num_tokens, b_flag);
            }
            else
            {
                fprintf(stderr, "\nERROR in line %d: Tokens %d >>>> %s\n\n", current_line, num_tokens, input_buf);
                exit(0);
            }

            current_line++;
        }

        traverse_label_tree(root);

        // Reset input and output file pointers
        rewind(ifptr);
        rewind(ofptr);
        finished_reading = 0;
        current_instruction = current_line = 1;
    }

    fclose(ifptr);
    fclose(ofptr);
}

void get_input(char ** argv, int argc, int * b_flag, char ** input_file_name, char ** output_file_name)
{
    char current_flag;              //  Current iterated flag
    int o_flag = 0;                 //  O flag (set output file name)
    int got_input_file = 0;         //  Got input file (necessary condition)
    int index;                      //  Index (used to check for non-option characters)

    //  Get input flags
    for (index = 1; index < argc; index++)
    {
        if(argv[index][0] == '-')
        {
            switch(argv[index][1])
            {
            case 'b':
                printf("Binary mode activated.\n");
                *b_flag = 1;
                break;
            case 'o':
                o_flag = 1;
                *output_file_name = argv[++index];
                printf("Output file name: %s\n", *output_file_name);
                break;
            default:
                fprintf(stderr, "default case reached.\n");
                break;
    
            }

        }
        else
        {
            *input_file_name = argv[index];
            got_input_file = 1;
        }
    }
    // while ((current_flag = getopt(argc, argv, "bo:")) != -1)
    // {
    //     switch (current_flag)
    //     {
    //         case 'b':
    //             printf("Binary mode activated.\n");
    //             *b_flag = 1;
    //             break;
    //         case 'o':
    //             o_flag = 1;
    //             *output_file_name = optarg;
    //             printf("Output file name: %s\n", *output_file_name);
    //             break;
    //         case '?':
    //             if (optopt == 'o')
    //             {
    //                 fprintf(stderr, "ERROR: INPUT_FORMAT_ERROR: Option -%c requires an argument.\n", optopt);
    //                 exit(INPUT_FORMAT_ERROR);
    //             }
    //             else
    //             {
    //                 fprintf(stderr, "ERROR: INPUT_FORMAT_ERROR: Unknown option -%c.\n", optopt);
    //                 exit(INPUT_FORMAT_ERROR);
    //             }
    //             break;
    //         default:
    //             fprintf(stderr, "default case reached.\n");
    //             break;
    //     }
    // }

    // //  Get file names
     // for (index = optind; index < argc; index++)
    // {
    //     printf("Non-option argument %d: %s\n", index, argv[index]);

    //     //  First non-option argument assumed to be input file name
    //     if (!got_input_file)
    //     {
    //         *input_file_name = argv[index];
    //         got_input_file = 1;
    //     }
    //     else
    //     {
    //         //  Already read input file name - incorrect number of args
    //         fprintf(stderr, "ERROR: INCORRECT_NUM_INPUTS: Incorrect number of arguments.\n");
    //         exit(INCORRECT_NUM_INPUTS);
    //     }
    // }

    if (!got_input_file)
    {
        //  Didn't get an input file name - missing input file name
        fprintf(stderr, "ERROR: MISSING_INPUT_FILE: Missing input file name.\n");
        exit(MISSING_INPUT_FILE);
    }
    else
    {
        //  Check input file has correct ending
        if (!verify_file_extension(*input_file_name, "asm"))
        {
            fprintf(stderr, "ERROR: INCORRECT_FILE_EXTENSION: Unexpected file extension. Input file must have '.asm' file extension.\n");
            exit(INCORRECT_FILE_EXTENSION);
        }
    }

    //  Make sure output file name is set
    if (!o_flag)
    {
        *output_file_name = *input_file_name;
    }

    //  Set ouput file name (with correct extension)
    *output_file_name = get_output_file(*output_file_name, *b_flag);
    
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

int fill_line_buffer(FILE *ifptr)
{
    /*  Fill line buffer with current line  */
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

int get_label() {
    //  Returns whether the current line contains a label
    if (input_buf[0] == ':')
    {
        return 1;
    }
    return 0;
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
    if (get_label())
    {
        //  Line starts with a label - return 0 tokens
        //printf("Line starts with a label\n");
        return 0;
    }
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

int num_digits(int n)
{
    //printf("input n: %d\n", n);
    int digits = 1;

    while ((n /= 10) > 0)
    {
        //printf("n: %d\n", n);
        digits++;
    }

    //printf("Integer: %d\tNum digits: %d\n", n, digits);

    return digits;
}

char * integer_to_imm_string(int n)
{
    //printf("INTEGER_TO_IMM_STRING()\n");
    int length = num_digits(n) + 2; // + 2 for '#' and '\0' characters
    int i;
    char * string = (char *)malloc(length * sizeof(char));

    string[0] = '#';
    for (i = length - 2; i >= 1; i--)
    {
        string[i] = (char) ('0' + (n % 10));
        n /= 10;
    }
    string[length - 1] = '\0';

    printf("Label address: %d\tImmediate string: %s\n", n, string);

    return string;
}

char * zero_imm = "#0";
char * get_label_address(Label * current, char * label_name)
{
    //printf("get_label_address()\n");
    char * label_address = NULL;
    if (current == NULL)
    {
        printf("Current is null, returning 0 immediate\n");
        return zero_imm;
    }

    printf("current->label: %s\tlabel_name: %s\n", current->label, label_name);

    if (strcmp(current->label, label_name) == 0)
    {
        //printf("Found label!\n");
        return integer_to_imm_string(current->instruction);
    }
    
    if (strcmp(current->label, label_name) < 0)
    {
        //printf("Traversing label to right child\n");
        return get_label_address(current->right, label_name);
    }

    //printf("Traversing label to left child\n");
    return get_label_address(current->left, label_name);
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
            case ':':
                param_seq[i] = immediate;
                //  Replace label with immediate address
                strcpy(tokens[i + 1], get_label_address(root, get_label_name(tokens[i + 1])));
                //tokens[i + 1] = get_label_address(root, tokens[i + 1]);
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
	int instruction_found = -1;
	
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
					instruction_found = 0;
					return 0;
                }
				// printf("Name and num param match but equal_sequences failed!\n");

            }
        }
    }
	
    return instruction_found;    
    
}

char * get_label_name(char * label)
{
    int i;
    int length = strlen(label);
    char * name = (char *)malloc(length);
    for (i = 1; i < length; i++)
    {
        name[i - 1] = label[i];
    }

    name[i] = '\0';

    return name;
}

Label * add_new_label()
{
    Label * new_label = (Label *)malloc(sizeof(Label));
    new_label->instruction = current_instruction;
    new_label->label = get_label_name(input_buf);
    new_label->left = new_label->right = NULL;

    return new_label;
}

void insert_label(Label * current)
{
    //  Add label to binary search tree
    //printf("insert_label()\n");

    if (current == NULL)
    {
        //printf("current is NULL\n");
        //  First label to be added
        root = add_new_label();

        return;
    }

    if (strcmp(current->label, get_label_name(input_buf)) == 0)
    //if (current->instruction == current_instruction)
    {
        //  Label already exists in BST
        return;
    }

    //if (current->instruction < (current_instruction))
    if (strcmp(current->label, get_label_name(input_buf)) < 0)
    {
        // Label should go to the right child
        if (current->right == NULL)
        {
            //  Add right child
            current->right = add_new_label();
        }
        else
        {
            insert_label(current->right);
        }
    }
    else
    {
        //  Label should go to the left child
        if (current->left == NULL)
        {
            //  Add left child
            current->left = add_new_label();
        }
        else
        {
            insert_label(current->left);
        }
    }
}

void traverse_label_tree(Label * current)
{
    if (current == NULL) return;
    traverse_label_tree(current->left);
    printf("Label: %s, instruction number %d\n", current->label, current->instruction);
    traverse_label_tree(current->right);
}

void write_line(uint8_t * instruction, FILE * ofptr, int num_tokens, int binary_flag) {

	// char comment[2] = "//";
	// char *current_char = input_buf;
	// int has_comment = 0;

	// while (isspace(*current_char))
	// 	current_char++;
	// if (*current_char == '/')
	// 	has_comment = 1;

    //printf("write_line()\n");

    if (num_tokens == 0 && input_buf[0] == ':')
    {
        //printf("Line started with a label\n");
        //  Insert label to label binary search tree
        insert_label(root);

        //printf("Root is null: %d\n", root == NULL);

        //printf("finished inserting label\n");

        if (!binary_flag)
        {
            //  Add comment of label
            fprintf(ofptr, "// %s\n",input_buf);
        }
    }
	else if (num_tokens == 0 && !binary_flag)
	{
        // Has a comment
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

