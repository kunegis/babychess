#include "parser.hh"

#include "verbose.hh"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

int Parser::skip_f_count;

#if 0
Parser::Parser(String new_text)
	:  text(new_text),
	   p(text())
{
}
#endif /* 0 */ 

String Parser::Skip_Word() 
{
	const char *old= current;
	while (*current && !isspace(*current))  ++current;
	return String(old, current-old);
}

bool Parser::Skip_String(String &result)
{
	result= ""; 
	if (*current != '\"')  return false;
	++ current;
	while (true)
	{
		char next= Next_Char();
		switch (next)
		{
		case '\0':
//		if (next == '\0')
			return false;
		case '\"':
//		if (next == '\"')
//		{
			++ current;
			return true;
//		}
		case '\\':
//		if (next == '\\')
//		{
			next= Skip_Char(); 
			/* Transform escape sequences */
//		}
		}
		result += next;
		++ current;
	}
}

String Parser::Next_Word_Excluding(String exclude)
{
	const char *q= current;
	while (q < begin+length                 &&
	       ! isspace(*q)                    &&
	       strchr(exclude(), *q) == NULL)
		++ q;
	return String(current, q-current);
}

String Parser::Skip_Excluding(char exclude)
{
	const char *q= strchr(current, exclude); 
	if (q == NULL)
		q= begin+length;
	String ret(current, q-current);
	current= q;
	return ret;
}

int Parser::Next_Format(const char *format, ...)
{
	va_list vl;
	va_start(vl, format);
	int ret= vsscanf(current, format, vl);
	va_end(vl);
	return ret; 
}

int Parser::Skip_Format_(const char *format, ...)
{
	static int n_count= -1;

	if (n_count < 0)
	{
		int a, b;
		n_count= sscanf("1", "%d%n", &a, &b)-1;
		Verbose("scanf() increments the return value by %d when using %%n.",
			n_count);
	}

	va_list vl;
	va_start(vl, format);
	int ret= vsscanf(current, format, vl)-n_count;
	va_end(vl);
	assert (current+skip_f_count <= begin+length);
	current += skip_f_count; 
	return ret; 
}
