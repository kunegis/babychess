#include "main.hh"
#include "format.hh"

int Main(int, char **)
{
	String ouch= Format("01234567890123456789012345678901"
			    "01234567890123456789012345678901x");
	if (*ouch != 65)   return 1;

	ouch= Format("a%d.%u", -1, 53);
	if (ouch != "a-1.53")   return 1;

	char b[100];
	memset(b, 'a', 99);
	b[99]= '\0';
	ouch = Format(b);
	if (ouch != b)   return 1; 
	
	return 0;
}
