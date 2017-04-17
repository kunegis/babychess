#include "opening_settings_stream.hh"

bool Read_Stream(FILE *file, Opening_Settings *settings)
{
	return 1 == fscanf(file, "%u", &settings->flags);
}

bool Write_Stream(FILE *file, const Opening_Settings *settings)
{
	return fprintf(file, "%u\n", settings->flags);
}

