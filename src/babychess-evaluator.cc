/* The chess evaluator.  Implements a "chess evaluator" as described
 * in `etc/CHESS-EVALUATOR'. 
 */

#include "string.hh"
#include "position.hh"
#include "score.hh"
#include "generator.hh"
#include "error.hh"
#include "material.hh"
#include "main.hh"
#include "time.hh"
#include "read_line.hh"
#include "square_value.hh"
//#include "parse_fen.hh"
#include "score_position.hh"
#include "move_score.hh"
#include "gettext.hh"
#include "knots.hh"
#include "arg_main.hh"
#include "fen.hh"

#include <limits.h>
#include <signal.h>
#include <stdlib.h>

#include <algorithm>

/* tuning */
enum {
	depth_step= 3,
};

/* options.  Set in the main function */ 
int option_verbose= 0;
bool option_interactive= false; 

#define Write_Verbose(args...)			\
{						\
	if (option_verbose)			\
	{					\
		fprintf(stderr,	                \
			args);		        \
	}					\
}

static Score_Position chess_evaluator_score_position;
/* initialized in Opt_Main() and used in Evaluate()  */


bool Quiet(const Move &m)
{
	bool result= (m.captured == pi_empty) && (m.promotion == pi_empty);
	return result;
}

/* Knot count.  Incremented in Evaluate().  Evaluate() outputs it
 * regularly.  It is also output once before the evaluator exits. 
 */
static knots_t knots= 0;

Inline void Write_Knots()
{
	printf("knots %"PRIknots"\n", knots);
}

void Evaluate(int depth,
	      int &score,
	      int &index,
	      int min,
	      int max)
/* Evaluate SCORE_POSITION.  Calculate DEPTH.  Write evaluation into SCORE.
 * If there are legal moves, write the best into INDEX, else set INDEX
 * to -1.  Calculate position from viewpoint of side to move. 
 * DEPTH= 0:   If all moves are quiet, return with the current score,
 *             else call Evaluate(-1, ...) for all moves and then
 *             Evaluate(0, ...) for only the best move. 
 *        >=1:  Call Evalute() for all depths -1...D-1 and all moves. 
 */
{
	assert (depth >= 0);

	/* Interactive mode */ 
	if (option_interactive)
	{
		printf(" > ");
		fflush(stdout);
		String line;
		if (Read_Line(stdin, line))
		{
			if (line == "quit")
			{
				Pointer_Halt(); 
				exit(1);
			}
		}
	}

	/* Count knots */ 
	++ knots;

	static int interval= 128; /* number of knots */ 

	if (knots % interval == 0)
	{
		static struct timeval last_time= {-1, 0};
		struct timeval now= Now();
		int elapsed= last_time.tv_sec >= 0 ? std::max(0, Date_Diff_ms(last_time, now)) : -1;

		if (elapsed != -1 && elapsed < 1000)
		{
			interval *= 2;
		}
		else if (elapsed >= 2000)
		{
			interval /= 2;
			if (interval < 1)  interval= 1;
		}
		Write_Knots();
		last_time= now;
	}

	/* Generate moves */ 
	Generator generator;
	generator.New(chess_evaluator_score_position, true);
	int count= *generator;

	/* Mate / Stalemate */ 
	bool in_check= chess_evaluator_score_position.In_Check();
	if (count == 0)
	{
		index= -1;
		score= in_check ? score_is_mate : score_even;
		return;
	}

	/* Quit at depth 0 with only quiet moves */ 
	if (depth == 0)
	{
		for (int i= 0; i < count; ++i)
			if (! Quiet(generator[i]))
				goto cont;

		score= chess_evaluator_score_position.Get_Score();
		Write_Verbose("\t\tposition is quiet.\t[%d]\n",
			      score); 
		index= 0;
		return;

	cont:
		int best_score= score_is_mate;
		int best_index= -1;
		for (int i= 0;  i < count;  ++i)
		{
			Move move= generator.Get_Move(i);
			int move_score= chess_evaluator_score_position.Get_Score()                
				+ Move_Score(move, 
					     chess_evaluator_score_position[move.from],
					     chess_evaluator_score_position.Get_Info().game_type)
					     + count;
			if (move_score > best_score)
			{
				best_score= move_score;
				best_index= i;
			}
		}
		assert (best_index >= 0);
		int new_score;
		int new_index;
		Score_Info score_info= chess_evaluator_score_position.Get_Info();
		chess_evaluator_score_position << generator[best_index];
		if (option_verbose)
		{
			String move= Simple_Notation(generator[best_index]); 
			fprintf(stderr,
				"\t-->  %s  (%d)\n",
				move(),
				chess_evaluator_score_position.Get_Score()); 
		}
		
		Evaluate(0, new_score, new_index, -max, -score_is_mate);
		chess_evaluator_score_position.Take_Back(generator[best_index], score_info);
		if (option_verbose)
		{
			Move move= generator[best_index]; 
			fprintf(stderr,
				"\t<--  %c%c-%c%c\n",
				'a' + File(move.to), '1' + Rank(move.to),
				'a' + File(move.from), '1' + Rank(move.from));
		}
		int last_score= Score_Negate(new_score);
		score= std::max(last_score, chess_evaluator_score_position.Get_Score());
		index= 0;
		return;
	}
	assert (depth > 0);

	/* Iterate through all moves */ 

	int order[count];

	int values[count];
	/* Real indexes */

	for (int i= 0;  i < count;  ++i)
		order[i]= i;

	/* make calculations for all depth beginning at -1 until
	 * D-1. */ 
	for (int d= -1;  
	     d < depth;   
	     d += depth_step, d + depth_step >= depth && d < depth - 1 + depth_step ? (d= depth - 1) : 0)
	{
		int bestval= score_is_mate;
		/* Check all moves */ 
		for (int i= 0; i < count; ++i)
		{
			if (d == -1) 
				/* Only check the move score */ 
			{
				Move m= generator.Get_Move(order[i]);
				values[order[i]]= 
					chess_evaluator_score_position.Get_Score()                
					+ Move_Score(m, 
						     chess_evaluator_score_position[m.from],
						     chess_evaluator_score_position.Get_Info().game_type)
					+ count;
			}
			else
			{
				/* play the move and evaluate that
				 * position */ 
				int old_score= chess_evaluator_score_position.Get_Score();
				Write_Verbose("\t\t(%d)\n", old_score); 
				Score_Info score_info= chess_evaluator_score_position.Get_Info();
				chess_evaluator_score_position << generator.Get_Move(order[i]);

				if (option_verbose)
				{
					String move= Simple_Notation(generator[order[i]]); 
					fprintf(stderr,
						"\t-->  %s  (%d)\n",
						move(),
						chess_evaluator_score_position.Get_Score()); 
				}

				int val;
				int ind;

				int minus= i > 2 ? depth_step-1 : 1;
				if (values[i] < values[0]-250)  minus += depth/2;

				Evaluate(std::max((d - minus), 0),
					 val,
					 ind,
					 - max,
					 std::min(- bestval, - min));

				if (option_verbose)
				{
					Move move= generator[order[i]];
					fprintf(stderr,
						"\t\t[%d]\n"
						"\t<--  %c%c-%c%c\n",
						val,
						'a' + File(move.to), '1' + Rank(move.to),
						'a' + File(move.from), '1' + Rank(move.from));
				}

				chess_evaluator_score_position.Take_Back(generator.Get_Move(order[i]), score_info);
				assert (old_score == chess_evaluator_score_position.Get_Score()); 

				if (depth == 0 && d == 0)
				{
					assert (i == 0);
					int last= Score_Negate(val);
					int current= chess_evaluator_score_position.Get_Score(); 
					Write_Verbose("\t\tlast=    %d, current= %d\n",
						      last, current);

					if (last > current)
					{
						score= last;
						Write_Verbose("\t\tassuming last move is best.\n");
					}
					else
					{
						score= current;
						Write_Verbose("\t\tlast move makes position worse.\n");
					}
					index= order[0];

					return; 
				}

				int newval= Score_Negate(val);
				Write_Verbose("\t\tval:  lastmove= %d, best= %d\n",
					      newval, bestval);
				if (newval > bestval)
				{
					Write_Verbose("\t\tnew best move.\n");
					bestval= newval;
				}

				values[order[i]]= newval;
				Write_Verbose("\t\tcutoff:  lastmove= %d, max= %d\n", newval, max);
				if (newval >= max)
				{
					Write_Verbose("\t\tcutting off.\n");
					for (int j= i + 1; j < count; ++j)
					{
						values[order[j]]= newval;
					}
					i= count;
				}
			}
		}
		
		for (int i= 0; i + 1 < count; ++i)
		{
			int best= i;
			for (int j= i + 1; j < count; ++j)
			{
				if (values[order[j]] > values[order[best]])
					best= j;
			}
			if (best != i)
			{
				int tmp= order[i];
				order[i]= order[best];
				order[best]= tmp;
			}
		}
	}

	index= order[0];
	score= values[index];

}

void Signal_Usr1(int)
{
	raise(SIGSTOP);
}

int Arg_Main(int argc, char **argv)
{
	signal(SIGUSR1, &Signal_Usr1);
	setvbuf(stdout, NULL, _IOLBF, 0);

	int c;
	String fen;
	int max_depth= INT_MAX;
	while ((c = getopt(argc, argv, "p:vid:")) >= 0)
	{
		switch (c)
		{
		case 'p':
			fen= optarg;
			break;
		case 'v':
			++ option_verbose;
			break;
		case 'i':
			option_interactive= true;
			break;
		case 'd':
			if (optarg == NULL)  break;

			if (1 != sscanf(optarg, "%d", & max_depth))
			{
				fprintf(stderr,
					"%s:  *** Error:  invalid argument \"%s\" to \"-d\".\n",
					program_invocation_name,
					optarg);
				return 1;
			}
			max_depth *= depth_step; 
			break;
		}
	}

	if (option_verbose)
	{
		fprintf(stderr,
			"\t%s:\n"
			"\t\t(%s)     %s\n"
			"\t\t[%s]     %s.\n"
			"\t\t--> %s   %s.\n"
			"\t\t<-- %s   %s.\n"
			"\n",
			_("Verbose mode output"),
			_("SCORE"),
			_("Score of position from the perspective of the side to move"),
			_("SCORE"),
			_("Score of position after calculation"),
			_("MOVE"),
			_("MOVE is played"),
			_("MOVE"),
			_("MOVE is taken back"));
	}

	if (option_interactive)
	{
		fprintf(stderr,
			_("\tInteractive mode:  Enter \"quit\" to exit.\n"));
		fprintf(stderr, "\n");
	}

	for (int i= optind;  i < argc;  ++i)
	{
		fen += argv[i];
		if (i + 1 < argc)
			fen += ' ';
	}

	Position position;

	if (fen.Is())
	{
		if (! Parse_FEN(fen, position))
		{
			String error= Error::Get();
			fprintf(stderr,
				"%s:  *** %s:  %s:  %s:  %s\n",
				program_invocation_name, 
				_("Error"),
				fen(),
				_("invalid FEN position"),
				error());
			return 1;
		}
	}
	else
		position.Reset();

	Generator generator;
	generator.New(position, false);

	if (*generator == 0)
	{
		if (position.In_Check())
			printf("score %%0\n");
		else
			printf("score 0-100\n");
		return 0;
	}

	chess_evaluator_score_position.Reset(position);

	for (int depth= depth_step;  depth < max_depth;  depth += depth_step)
	{
		printf("depth %d\n", depth / depth_step);
		Write_Knots();

		int score;
		int index;

		Evaluate(depth,
			 score,
			 index,
			 score_is_mate,
			 score_mate_in_1);

		String move= generator.Get_Simple_Move_Name(index);

		printf("main %s\n", move());

		String evaluation= Format_Score(score);

		printf("move %s %s\n", move(), evaluation());
		
		String score_text= Format_Score(score);
		printf("score %s\n", score_text());

		if (score > score_not_win    ||
		    score < score_not_lose)
		{
			Write_Knots();

			return 0;
		}
	}

	Write_Knots();

	return 0; 
}

void Arg_Print_Help()
{
	printf("%s.\n"
	       "\n"
	       "%s\n"
	       "\n"
	       "%s\n"
	       "\t-p \"%s\"    %s\n"
	       "\t-v               %s\n"
	       "\t-i               %s\n"
	       "\t-d %s         %s\n",
	       _("The BabyChess evaluator"),
	       _("Analyze a chess position, and output thinking information.  "
		 "Stop as soon as a mate is found."),
	       _("Options:"),
	       _("POSITION"),
	       _("The position to analyze, in FEN notation.  "
		 "If not specified, use the standard starting position."),
	       _("Verbose mode.  Write additional analyze information on the error output."),
	       _("Interactive mode.  Read one line of input before each step."),
	       _("DEPTH"),
	       _("Calculate up to depth DEPTH.  A value of -1 indicates to not stop.  "
		 "Default is -1."));
}
