#include "evaluator_xboard_server.hh"

#include "fatal.hh"
#include "gettext.hh"
#include "simple_poll.hh"

void Evaluator_XBoard_Server::Run(const char *engine,
				  const char *fen,
				  int verbose)
{
	if (! Open(engine, verbose))
	{
		Fatal(Format(_("Starting \"%s\""), engine));
	}

	printf("pid %d\n", (int)PID());

	Set_Post();
	if (fen)
		Set_FEN(fen);
	Analyze(); 

	while (! XBoard_Server::Get_Quit())
		Simple_Poll(); 
}

void Evaluator_XBoard_Server::XS_Analyze(unsigned depth,
					 int score,
					 knots_t knots,
					 String best)
{
	printf("depth %u\n", depth);
	printf("knots %"PRIknots"\n", knots);
	printf("main %s\n", best());
	printf("score %d\n", score);
}
