#include "move_score.hh"

#include "piece_score.hh"
#include "square_score.hh"

int Move_Score(const Move &move,
	       int piece,
	       int game_type)
{
	int result= 0;

	/* capture */ 
	if (move.captured & pi_color)
		result += Piece_Score(move.Captured_Square(), 
				      move.captured, 
				      game_type);

	/* special */ 
	if (move.promotion & pi_color)
	{
		result += Piece_Score(move.to, move.promotion, game_type);
		result -= Piece_Score(move.from, piece, game_type);
	}
	else
	{
		bool black= piece & pi_black;
		result += Square_Score(move.to, piece, game_type);
		result -= Square_Score(move.from, piece, game_type);
		if (move.castling)
		{
			result += Square_Score(move.Rook_To(),
					       Piece(black, pi_R), game_type);
			result -= Square_Score(move.Rook_From(),
					       Piece(black, pi_R), game_type);
		}
	}

	return result;
}
