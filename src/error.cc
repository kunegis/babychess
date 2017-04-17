#include "error.hh"

#include "gettext.hh"
#include "format.hh"

#include <errno.h>

String Error::text, Error::filename;

String Error::Get()
{
	String message;
	if (errno == -1)       message= text;
	else if (errno != 0)   message= strerror(errno);
	else
	{
		/* There is no error.  Either this function was called
		 * when there is no error or a function didn't set
		 * errno. */ 
		assert (0);
		message= "Error";
	}
	if (filename)
	{
		String ret= Format(_("\"%s\""), filename());
		return Format(_("%s:  %s"), ret(), message());
	}
	else
	{
		return message; 
	}
}

void Error::Set(String new_text,
		String new_filename)
{
	text= new_text;
	filename= new_filename; 
	errno= -1;
}
