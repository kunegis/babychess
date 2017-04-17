#ifndef STRNLEN_HH
#define STRNLEN_HH

#include "../config.hh"

#if HAVE_STRNLEN

#include <string.h>

#else /* ! HAVE_STRNLEN */ 

#include "inline.hh"
#include "verbose.hh"

/* This implementation should be replaced */ 
Inline size_t strnlen (const char *c, size_t len)
{
	Verbose("strnlen(\"%s\", %u)", c, len);
	size_t ret= 0;
	while (ret < len && *c)  
		{
			++ret;
			++c;
		}
	return ret;
}

#endif /* ! HAVE_STRNLEN */ 

#endif /* ! STRNLEN_HH */ 
