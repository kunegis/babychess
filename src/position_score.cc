#include "position_score.hh"

#include "score.hh"
#include "piece_score.hh"

int Position_Score(const Position &position,
		   int game_type)
{
	int result= score_even;
	
	for (int file= sq_a; file <= sq_h; ++ file)
		for (int rank= sq_1; rank <= sq_8; ++ rank)
		{
			int piece= position[Make_Square(file, rank)];
			if (piece & pi_color)
			{
				int val= Piece_Score(Make_Square(file, rank),
						     piece,
						     game_type);
				if (piece & pi_white)
					result += val;
				else 
					result -= val;
				
			}
		}

	return result;
}
