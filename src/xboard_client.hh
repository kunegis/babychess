#ifndef XBOARD_CLIENT_HH
#define XBOARD_CLIENT_HH

#include "input_reader.hh"
#include "default_timer.hh"
#include "evaluator.hh"
#include "game.hh"
#include "opening_book.hh"

class XBoard_Client
	:  public Input_Reader,
	   public Evaluator
{
public:
	void Run();

private:

	Game game; 
	bool quit;
	bool post;

	Opening_Book opening_book; 

	virtual void IR_Input();
	/* Standard input */

	void Process_Input(String line); 

	void EC_Stop() ;

	void EC_Timer(int /* date_milliseconds */,
		      int /* time_milliseconds */)
		{ }
	void EC_Run()
		{ }
	void EC_Pause()
		{ }
	void EC_Depth(int)
		{ }
	void EC_Main(int); 

	void Play(int move);
	bool Open_Opening_Book();
	void Play_Or_Start();
};

#endif /* ! XBOARD_CLIENT_HH */
