#include "time.hh"

#include <time.h>
#include <string.h>

String Format_Time(struct timeval time, const char *format)
{
	time_t tm= time.tv_sec;

	struct tm *t= localtime(& tm);  
	if (t == NULL)  return "";

	enum {size= 0x100};
	char result[size];
	if (0 == strftime(result, size, format, t))
		return "";
	return result; 
}

struct timeval Parse_Time(const char *text, const char *format)
{
	assert (text);

	struct tm tm;
	memset(& tm, '\0', sizeof (tm));
	const char *st= strptime(text, format, & tm);
	if (st == NULL)  
	{
		return (struct timeval) {-1, 0};
	}

	time_t result= mktime(& tm);
	return Make_Date(result);
}
