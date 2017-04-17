#include "main.hh"
#include "strnlen.hh"
#include "verbose.hh"

#include <stdio.h>

int Main(int, char **)
{
	struct Entry {const char *text;  size_t len; };
	Entry strings[]=
		{{"", 0},
		 {"1", 1},
		 {"0123456789", 10},
		 {"01234567890123", 10}};
	int ret= 0;

	for (int i= 0;  i < (int) (sizeof(strings)/sizeof(Entry));  ++i)
	{
		Verbose("testing strnlen(\"%s\", %u)", strings[i].text, strings[i].len);
		size_t result= strnlen(strings[i].text, 10);
		Verbose("\t= %u", result);
		if (result != strings[i].len)
		{
			fprintf(stderr, "*** error:  strnlen(\"%s\") != %u\n",
				strings[i].text,
				strings[i].len);
				
			ret |= 1;
		}
	}

	return ret;
}
