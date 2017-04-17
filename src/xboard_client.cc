#include "xboard_client.hh"

#include "parser.hh"
#include "simple_poll.hh"
#include "read_line.hh"
#include "notation.hh"
#include "write_text_position.hh"
#include "format_move_name.hh"

#include <unistd.h>
#include <fcntl.h>

#include <algorithm>

void XBoard_Client::Process_Input(String line)
{
	Parser parser(line);
	parser.Skip_Space();
	String word= parser.Skip_Word();
	if (word == "quit")
	{
		quit= true; 
	}
	else if (word == "go")
	{
		Reset();
		Play_Or_Start();
	}
	else if (word == "new")
	{
		Reset();
		game.New();
	}
	else if (word == "?")
	{
		if (Evaluator::Started())
		{
			assert (*game.Current_Generator() > 0);
			int best_move= Evaluator::Best_Move();
			Reset();
			Play(best_move);
		}
	}
	else if (word == "time")
	{
		parser.Skip_Space();
		String time= parser.Skip_Word();
		int time_cs;
		if (word.Is() && 1 == sscanf(time(), "%d", & time_cs))
		{
			int time_s= std::max(time_cs / (25 * 100), 1); 
			Verbose("time= %d s", time_s); 
			settings.timing= ti_time;
			settings.time_s= time_s; 
		}
		else
		{
			printf("Error (wrong parameter): wrong parameter\n");
		}
	}
	else if (word == "post")
	{
		post= true; 
	}
	else if (word == "nopost")
	{
		post= false;
	}
	else if (word == "remove")
	{
		/* The user wants to take back his last move.  If the evaluator
		 * is running, then we are to play, and we only take back the
		 * user's last move.  If the user is to play, we take back our
		 * last move and the user's last move. 
		 */
		if (game.Overall_Location() == 0)
			return;
		bool running= Evaluator::Started();
		Evaluator::Reset();
		game.Go_Backward();
		if (! running && game.Overall_Location() != 0)
			game.Go_Backward();
	}
	else if (word == "xboard")
	{
		/* Ignore, because we're always in XBoard mode */ 
	}
	else if (word == "edit")
	{
		Position p= game.Current_Position();
		
	}

	/* Extensions */
	else if (word == "extension_position")
	{
		Write_Text_Position(stdout, game.Current_Position());
	}

	else /* move name */ 
	{
		int move_index= game.Current_Generator().Simple_Notation_Move(word); 
		if (move_index >= 0)
		{
			Verbose("\tScanned as #%d.", move_index);
			Reset();
			game.Append_New_Move(move_index);
			if (*game.Current_Generator() == 0)
			{
				if (game.Current_Position().In_Check())
				{
					if (game.Current_Position().To_Move() == cc_w)
						printf("0-1 {Black mates}\n");
					else
						printf("1-0 {White mates}\n");
				}
				else
				{
					printf("1/2-1/2 {Stalemate}\n");
				}
			}
			else if (*game.Current_Generator() == 1)
			{
				String move_text= Format_Move_Name
					(game.Current_Generator().Get_Move_Name(0),
					 san_notation,
					 san_notation_settings); 
				printf("move %s\n", move_text());
				game.Append_New_Move(0);
			}
			else
			{
				Play_Or_Start();
			}
		}
		else
		{
			printf("Error (unknown command or no such move): %s\n", word());
		}
	}
}

void XBoard_Client::Run()
{
	/* Set stdin to not block */ 
	int flags= fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, flags | O_NONBLOCK);


	Input_Reader::Open(0); 

	game.New();

	Evaluator::Open(game.Current_Position(), game.Current_Generator()); 

	Evaluator::settings.Reset();
	Evaluator::settings.timing= ti_time;
	Evaluator::settings.time_s= 5;
	Evaluator::settings.mode= em_opponent;
	Evaluator::settings.flags= evf_no_nice/*|evf_use_openings*/;
	/* We're not interactive */ 

	quit= false;
	post= false;
			
	while (! quit)
	{
		Simple_Poll(); 
	}

	if (Evaluator::Started())
		Evaluator::Reset();
	Evaluator::Close(); 
}

void XBoard_Client::IR_Input()
{
	String line;
	while (Read_Line(stdin, line))
	{
		if (line.Is())
		{
			Process_Input(line);
		}
	}
}


void XBoard_Client::EC_Stop()
{
	int best_move= Evaluator::Best_Move();

	if (best_move < 0)  return;

	String text= game.Current_Generator().Get_Simple_Move_Name(best_move);
	printf("move %s\n", text());
	game.Append_New_Move(best_move);
	Evaluator::Reset();
}


void XBoard_Client::EC_Main(int)
{
	if (! post)  return;
	String move= Format_Move_Name
		(game.Current_Generator().Get_Move_Name(Evaluator::Best_Move()),
		 san_notation,
		 san_notation_settings);
	printf("%d %d %d %d %s\n",
	       Evaluator::Depth(),
	       Evaluator::Score(),
	       Evaluator::Age_ms() / 10,
	       Evaluator::Knots(),
	       move());
}

void XBoard_Client::Play(int move)
{
	String text= Format_Move_Name
		(game.Current_Generator().Get_Move_Name(move),
		 san_notation,
		 san_notation_settings); 
	game.Append_New_Move(move);
	printf("move %s\n", text());
}

bool XBoard_Client::Open_Opening_Book()
{
	if (opening_book.Is())  return true; 
	return opening_book.Open();
}

void XBoard_Client::Play_Or_Start()
{
	if (*game.Current_Generator() == 0)
		return;
	if (*game.Current_Generator() == 1)
	{
		Play(0);
		return; 
	}
	if (Open_Opening_Book())
	{
		if (opening_book.Get(game))
		{
			move_t small_move;
			bool cro=
				opening_book.Choose_Random_Opening(small_move, true);
			if (cro)
			{
				Play(game.Current_Generator().Small_Move(small_move)); 
				return;
			}
		}
	}
	Evaluator::Start(); 
}

