#include "piece_score.hh"

#include "piece_value.hh"
#include "square.hh"
#include "square_score.hh"

int Piece_Score(int square,
		int piece,
		int game_type)
{
	return Piece_Value(piece & pi_type) 
		+ Square_Score((piece & pi_white) ? square : Square_Invert(square),
			       piece, 
			       game_type); 
}

