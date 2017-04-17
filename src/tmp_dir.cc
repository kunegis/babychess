#include "tmp_dir.hh"

#include <stdlib.h>

const char *Tmp_Dir()
{
	static const char *ret;
	
	if (! ret)
	{
		ret= getenv("TMP") ?: "/tmp";
	}

	return ret; 
}

