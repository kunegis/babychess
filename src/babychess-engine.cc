/* The XBoard chess engine 
 */

/* A chess engine compatible to XBoard.
 */

#include "arg_main.hh"
#include "xboard_client.hh"
#include "gettext.hh"

#include <signal.h>

int Arg_Main(int,  char **)
{
	/* The protocol says that the server may send SIGINT.  Ignore that. */ 
	signal(SIGINT, SIG_IGN);

	/* Set output to line buffered */ 
	setvbuf(stdout, NULL, _IOLBF, 0);

	XBoard_Client xboard_client;
	xboard_client.Run(); 

	return 0;
}

void Arg_Print_Help()
{
	String see= Format(_("This program implements an XBoard engine.  See %s"),
			   "http://www.tim-mann.org/xboard/engine-intf.html");
	printf("%s.\n"
	       "\n"
	       "%s\n",
	       _("The BabyChess XBoard engine"),
	       see());
}
