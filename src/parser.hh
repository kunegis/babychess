#ifndef PARSER_HH
#define PARSER_HH

#include "string.hh"
#include "pointer.hh"

#include <ctype.h>

class Parser
{
public:
//	Parser(String text= "");
	/* Parse TEXT.  TEXT may be empty. 
	 */
	Parser(const char *text= "", int l= -1)
		:  begin(text),
		   current(text),
		   length(l < 0 ? strlen(text) : l)
		{
		}
		
	Parser(String s)
		:  string_text(s),
		   begin(s()),
		   current(s()),
		   length(*s)
		{
		}

	bool End() const
		{ return  current == begin + length; }

	/* The Skip_*() functions advance the index and possibly
	 * return what was read.
	 *
	 * The Next_*() functions read ahead but don't modify the
	 * index. 
	 */

	void Skip_Space()
		{
			while (isspace(*current))  ++current;  
		}
	String Skip_Word();
	bool Skip_String(String &result);
	/* Read a string enclosed in quotes.  If the result is FALSE:
	 * If RESULT is empty, the next character is not a quote, else
	 * the string is unterminated and RESULT contains the string. 
	 */
	char Next_Char() const 
		{ return *current; }
	char Skip_Char()
		{
			if (!*current)  return '\0';
			else            return *(current++);
		}
	void Skip(int n)
		{
			assert (n >= 0);
			assert (current+n <= begin+length);
			current += n;
		}
	bool Skip(const char *text)
		{
			size_t len= strlen(text);
			if (strncmp(current, text, len))
				return false;
			current += len;
			return true;
		}
	
	String Next_Word_Excluding(String exclude);
	String Skip_Word_Excluding(String exclude)
		{
			String result= Next_Word_Excluding(exclude);
			Skip(*result);
			return result; 
		}

	String Skip_Excluding(char exclude);
	const char *operator() () const 
		{
			return current; 
		}

	/* Formatted parsing, using scanf() */
	int Next_Format(const char *format, ...)
		__attribute__ ((format (scanf, 2, 3)));
#define Skip_Format(format, args...)  Skip_Format_(format "%n" , ## args, &Parser::skip_f_count)
	int Skip_Format_(const char *format, ...);
	static int skip_f_count;

	char operator [] (int i) const 
		{
			assert (i >= 0);
			assert (current+i <= begin+length);
			return current[i];
		}
	String Text() const
		{
			if (string_text)
				return string_text;
			else
				return String(begin, length);
		}

private:
	const String string_text;

	const char *const begin, *current;
	/* points into TEXT */ 
	const int length;
};

#endif /* ! PARSER_HH */
