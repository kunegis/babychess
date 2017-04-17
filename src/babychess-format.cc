#include "arg_main.hh"
#include "gettext.hh"

#include <stdio.h>

int Arg_Main(int /* argc */ , char ** /* argv */ )
{
	return 0; 
}

void Arg_Print_Help()
{
	printf(
		"%s.\n"
		"\n"
		"%s\n"
		"\n"
		"%s\n"
		,
		_("The BabyChess formatting program"),
		_("Take a PGN or FEN file and export it to text or HTML."),
		_("Options:")
		);
}

