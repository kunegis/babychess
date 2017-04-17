#include "main.hh"

/* Test notation in all settings and differrent moves.  Above are a list
 * of moves (in form of Move_Name) and a list of notation settings.  The
 * function Format_Move_Name() is called for all combinations. The
 * output is compared to the correct strings in
 * share/test_notation/data.  
 */

#include "format_move_name.hh"

int Main(int, char **)
{
	Move_Name names[]= {
		{pi_P, 0,  4, 1, 4, 3,  0, 0, 0, 0, 0}, 
		{pi_N, 0,  6, 0, 5, 2,  0, 0, 0, 0, 0},
		{pi_P, 1,  4, 3, 3, 4,  0, 0, 0, 0, 0},
		{pi_P, 1,  4, 4, 3, 5,  1, 0, 0, 0, 0},
		{pi_B, 1,  2, 3, 5, 6,  0, 0, 1, 0, 0},
		{pi_R, 2,  0, 0, 0, 7,  0, 0, 0, 0, 0},
		{pi_N, 0,  4, 3, 3, 5,  0, 0, 2, 0, 0},
		{pi_K, 0,  4, 0, 6, 0,  2, 0, 0, 0, 0},
		{pi_K, 0,  4, 7, 2, 7,  3, 0, 0, 0, 1},
		{pi_P, 0,  7, 6, 7, 7,  7, 0, 0, 0, 0},
		{pi_P, 4,  1, 1, 0, 0,  4, 0, 1, 0, 1},
		{pi_N, 0,  1, 7, 3, 6,  0, 1, 0, 0, 1},
		{pi_R, 0,  4, 4, 4, 6,  0, 2, 0, 0, 0},
		{pi_Q, 0,  3, 2, 1, 2,  0, 3, 0, 0, 0},
		{pi_Q, 0,  3, 7, 7, 3,  0, 0, 1, 1, 1},
		{pi_B, 0,  5, 5, 6, 6,  0, 0, 2, 1, 0},
	};

	Notation_Settings settings[]= {
		Notation_Settings(n_short, nos_no_ep|nos_no_double_check),      
		Notation_Settings(n_long,  nos_no_ep|nos_no_double_check),
		Notation_Settings(n_short, nos_pawn_sign|nos_no_ep|nos_no_double_check),
		Notation_Settings(n_long,  nos_pawn_sign|nos_no_ep|nos_no_double_check),
		Notation_Settings(n_short, nos_no_double_check),
		Notation_Settings(n_short, nos_no_check|nos_no_ep|nos_no_double_check),
		Notation_Settings(n_short, nos_no_short_capture|nos_no_ep|nos_no_double_check),
		Notation_Settings(n_short, nos_short_capture_end|nos_no_ep|nos_no_double_check),
		Notation_Settings(n_long,  nos_long_capture_name|nos_no_ep|nos_no_double_check),
		Notation_Settings(n_long,  nos_pawn_sign|nos_long_capture_name|nos_no_ep|nos_no_double_check),
		Notation_Settings(n_short, nos_no_ep),
		Notation_Settings(n_short, nos_no_castling|nos_no_ep|nos_no_double_check),
		Notation_Settings(n_long,  nos_no_castling|nos_no_ep|nos_no_double_check),
	};

	enum {n= sizeof(names)/sizeof(Move_Name),
	      s= sizeof(settings)/sizeof(Notation_Settings)};

	const char *strings[n][s]= {
#include "../bin/test_notation_data.c"
	};

	int ret= 0;

	for (int i= 0; i < n;  ++i)
		for (int j= 0;  j < s;  ++j)
		{
			String t= Format_Move_Name(names[i], san_notation, settings[j]);
			if (t != strings[i][j])
			{
				fprintf(stderr, "[%d][%d]  \"%s\" != \"%s\"\n",
					i, j,
					t(),
					strings[i][j]);
				ret |= 1;
			}
		}

	return ret;
}
