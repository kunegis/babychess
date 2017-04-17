#include "version_number.hh"

#include "string.hh"
#include "time.hh"

const char *Version_Date()
{
	static String ret;
	if (ret)  return ret(); 

	struct timeval time= Parse_Time(__DATE__, "%b %d %Y");
	ret= Format_Time(time, "%x");

	return ret();
}

