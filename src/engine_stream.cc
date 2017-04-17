#include "engine_stream.hh"

bool Read_Stream(FILE *file, Engine *engine)
{
	char type, program[256];
	int f= fscanf(file, "%c%256s", &type, program);
	if (f < 1)
		return false;
	if (! (type >= 0 && type < et_count))
		return false;
	if (f == 1)
		program[0]= '\0';
	engine->type= type;
	engine->program= program; 
	return true;
}

bool Write_Stream(FILE *file, const Engine *engine)
{
	char c= engine->type;
	fprintf(file, "%c%s", c, engine->program());
	return true;
}
