#include "squares_touched.hh"

void Squares_Touched(const Move &move, int *squares) 
{
	squares[0]= move.from;
	squares[1]= move.to;

	if (move.ep)
	{
		squares[2]= move.ep;
		squares[3]= 0;
	}
	else if (move.castling)
	{
		switch (move.castling)
		{
		case wO_O:   squares[2]= sq_h1; squares[3]= sq_f1; break;
		case wO_O_O: squares[2]= sq_a1; squares[3]= sq_d1; break;
		case bO_O:   squares[2]= sq_h8; squares[3]= sq_f8; break;
		case bO_O_O: squares[2]= sq_a8; squares[3]= sq_d8; break;

		default: assert (0);
		}
	}
	else
	{
		squares[2]= 0;
		squares[3]= 0;
	}
}
