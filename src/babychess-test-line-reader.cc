#include "main.hh"
#include "line_reader.hh"

int Main(int, char **)
{
	FILE *file= tmpfile();
	if (file == NULL)  return 1;
	fprintf(file, "Hello, World!\n");
	enum {len= 0x1000};
	for (int i= 0; i < len;  ++i)
		if (0 > fputc('J', file))  return 1;
	if (0 > fputc('\n', file))  return 1;
	rewind (file);

	Line_Reader line_reader(file);

	if (! line_reader.Read())  return 1;
	if (strcmp(line_reader.Line(), "Hello, World!"))  return 1;
	if (! line_reader.Read())  return 1;
	if (line_reader.Length() != len)  return 1;
	for (int i= 0; i < len; ++i)
		if (line_reader.Line()[i] != 'J')  return 1;

	if (line_reader.Read())   return 1;
	
	return 0;
}
