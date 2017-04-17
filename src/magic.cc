#include "magic.hh"

#include "stdio.hh"

bool Magic::Write(FILE *file, const char *name)
{
	assert (name && strlen(name) <= max_len);
	return 0 <= fprintf(file, "babychess.%s\n", name);
}

bool Magic::Read (FILE *file, const char *name)
{
	char buf[max_len+11];
	if (NULL == stdio_fgets(buf, max_len+11, file))
		return false;
	if (memcmp(buf, "babychess.", 10))  
		return false;
	
	char *eol= strchr(buf+10, '\n');
	if (eol == NULL)  return false;
	*eol= '\0';
	return 0 == memcmp(buf+10, name, eol-buf-10);
}
