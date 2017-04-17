#ifndef MAGIC_HH
#define MAGIC_HH

#include <stdio.h>

class Magic
{
public:
	enum {max_len= 30};

	static bool Write(FILE *, const char *name);
	static bool Read (FILE *, const char *name);
};

#endif /* ! MAGIC_HH */
