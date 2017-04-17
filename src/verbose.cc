#include "verbose.hh"

#ifndef NDEBUG

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#ifndef VERBOSE_ENV_VAR_NAME
#    define VERBOSE_ENV_VAR_NAME  "BABYCHESS_VERBOSE_FD"
#endif

FILE *verbose_file;

bool Verbose_Enabled(const char *source_filename)
{
	int len= strlen(source_filename);
	char name[len+2];
	memcpy(name, source_filename+4, len-7);
	for (int i= 0; i < len-7; ++i)
		name[i]= name[i] == '-' ? '_' : toupper(name[i]);
	memcpy(name+len-7, "_VERBOSE", 9);
	const char *env= getenv(name);

	if (env == NULL)   return false;

	if (verbose_file == NULL)
	{
		int fd;
		const char *fd_text= getenv(VERBOSE_ENV_VAR_NAME);
		if (fd_text == NULL)
		{
			fd= dup(fileno(stderr));
			if (fd < 0)    return false;
			static char text[]= VERBOSE_ENV_VAR_NAME
				"=\0\0\0\0\0\0\0\0\0\0";
			sprintf(text+strlen(VERBOSE_ENV_VAR_NAME)+1, "%u", fd);
			putenv(text);
		}
		else
		{
			if (1 != sscanf(fd_text, "%u", &fd))
				return false;
		}
		verbose_file= fdopen(fd, "a");
		if (verbose_file == NULL)
			return false;
	}

	return true; 
}

#endif /* ! NDEBUG */ 
