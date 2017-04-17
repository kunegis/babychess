#ifndef PIECE_VALUE_HH
#define PIECE_VALUE_HH

Inline int Piece_Value(int pt)
{
	assert (pt >= pi_P && pt <= pi_K);

	static const int values[6]= {100, 300, 300, 450, 900, 0};

	return values[pt];
}

#endif /* ! PIECE_VALUE_HH */ 
