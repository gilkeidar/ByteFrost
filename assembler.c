#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE_LENGTH	300

int fill_line_buffer(char * input_buf, FILE *ifptr);

int write_machine_code(char *input_buf, FILE *ofptr);



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
	printf("successfully opened input file\n");

	ofptr = fopen(output_file, "w");	//	Create output file

	//	Loop to read through file line-by-line
	char * input_buf = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));	//	Input buffer (for each line in text file)
	char curr_char;
	while (fill_line_buffer(input_buf, ifptr))
	{
		printf("writing machine code\n");
		write_machine_code(input_buf, ofptr);	//	Convert Assembly language line to machine language and write to output file
	}

	printf("exited loop\n");

	free(input_file);
	free(output_file);
	free(input_buf);
	
	fclose(ifptr);
	fclose(ofptr);

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
	printf("Current line contents: %s |\n", input_buf);


	return (curr_char != EOF);
}




int write_machine_code(char *input_buf, FILE *ofptr)
{
	return 0;
}
