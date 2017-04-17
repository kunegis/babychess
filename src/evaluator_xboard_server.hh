#ifndef EVALUATOR_XBOARD_SERVER_HH
#define EVALUATOR_XBOARD_SERVER_HH

#include "xboard_server.hh"

class Evaluator_XBoard_Server
	:  public XBoard_Server
{
public:
	void Run(const char *engine,
		 const char *fen,
		 int verbose);
	/* If FEN is NULL, analyze the standard starting position. 
	 */

private:
	void XS_Analyze(unsigned, int, knots_t, String);
};

#endif /* ! EVALUATOR_XBOARD_SERVER_HH */ 
