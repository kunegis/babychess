#include "arg_main.hh"
#include "gettext.hh"
#include "main.hh"
#include "evaluator_xboard_server.hh"
#include "error.hh"
#include "fatal.hh"
#include "process.hh"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void Signal_Cont(int);
void Signal_Usr1(int)
{
	if (process_last_pid >= 0)
		kill (process_last_pid, SIGSTOP);
	signal(SIGCONT, &Signal_Cont);
	raise(SIGSTOP);
}

void Signal_Cont(int)
{
	if (process_last_pid >= 0)
		kill (process_last_pid, SIGCONT);
	signal(SIGUSR1, &Signal_Usr1);
}

int Arg_Main(int argc, char **argv)
{
	const char *position= NULL;
	int verbose= 0;

	int c;
	while ((c= getopt(argc, argv, "p:v")) >= 0)
	{
		switch (c)
		{
		case 'p':
			position= optarg; 
			break;
		case 'v':
			verbose= 1;
			break;
		}
	}

	const char *engine= argv[optind];
	if (!engine)
	{
		Error::Set(_("No engine specified"));
		Fatal(); 
	}

	/* handle SIGUSR1 */
	signal(SIGUSR1, &Signal_Usr1);

	/* Set output to line buffered */ 
	setvbuf(stdout, NULL, _IOLBF, 0);

	Evaluator_XBoard_Server server;
	server.Run(engine, position, verbose); 

	return 0;
}

void Arg_Print_Help()
{
	printf("%s\n"
	       "\n"
	       "%s\n"
	       "\t%s %s [%s]\n"
	       "\n"
	       "%s\n"
	       "\t-p \"%s\"       %s\n"
	       "\t-v              %s\n"
	       ,
	       _("This program implements a BabyChess evaluator using an XBoard engine."),
	       _("Invocation:"),
	       program_invocation_name,
	       _("XBOARD-ENGINE-PROGRAM-NAME"),
	       _("OPTIONS"),
	       _("Options:"),
	       _("POSITION"),
	       _("The position to analyze, in FEN notation.  "
		 "If not specified, use the standard starting position."),
	       _("Enabled verbose mode"));
}
