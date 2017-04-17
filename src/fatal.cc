#include "fatal.hh"

#include "error.hh"
#include "gettext.hh"
#include "main.hh"

#include <stdio.h>
#include <libgen.h>

void Fatal(String text)
{
	String error= Error::Get();
	fprintf(stderr,
		text.Is() 
		? _("%s: *** error:  %s:  %s\n")
		: _("%s: *** error:  %s\n"),
		program_invocation_short_name,
		text.Is() ? text() : error(),
		error());
	exit(1);
}
