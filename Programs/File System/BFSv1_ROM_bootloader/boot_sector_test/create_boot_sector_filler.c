#include <stdio.h>
main()
{
	int i, j, k;
	FILE* file_ptr;
	const char* filename = "disk";
	file_ptr = fopen(filename, "wb");
	 
	if (file_ptr == NULL) {
		perror("Error opening file for writing");
		return -1;
	}


	for (i = 1; i <= 8; i++)
	{
		for (j = 0; j < 256; j++)
		{
			k = i * 16 + (j % 16);
			printf("%x ", k);
			fwrite(&k, 1, 1, file_ptr);
		}
	}
	fclose(file_ptr);
}