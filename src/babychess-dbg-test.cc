#include "cassert.hh"
#include "main.hh"

int Main(int c, char **)
{
	enum {a= 5};
	cassert (a == 5);
	cassert (c == 1);
	return 0;
};
       
