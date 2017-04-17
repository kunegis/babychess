#ifndef RANDOMIZE_HH
#define RANDOMIZE_HH

#include "inline.hh"

#include <stdlib.h>
#include <time.h>

extern bool randomized; 

Inline void Randomize()
{
	if (! randomized)
		srand(time(NULL)); 
}

#endif /* ! RANDOMIZE_HH */
