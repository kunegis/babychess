#include <stdio.h>
#include <string.h>

#define STRING_1 "Hello, World!\n"
#define A "aaaaaaaaaaaaaaaaaa"
#define B A A A A A A A A A A
#define STRING_2 B B B B B B B B B B B

int main()
{
	FILE *file= tmpfile();
	if (file == NULL)  return 1;
	if (0 > fputs(STRING_1 STRING_2, file))
		return 1;
	rewind(file);
	char *line= NULL;
	size_t n= 0;
	if (getline(&line, &n, file) != strlen(STRING_1))
		return 1;
	if (strcmp(line, STRING_1)) 
		return 1;
	if (getline(&line, &n, file) != strlen(STRING_2))
		return 1;
	if (strcmp(line, STRING_2)) 
		return 1;

	return 0;
}
