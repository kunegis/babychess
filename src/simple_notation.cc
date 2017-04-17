#include "simple_notation.hh"

String Simple_Notation(const Move &move)
{
	static const char english_chars[]= {'n', 'b', 'r', 'q'};
	char r[6];

	r[0]= 'a' + File(move.from);
	r[1]= '1' + Rank(move.from);
	r[2]= 'a' + File(move.to);
	r[3]= '1' + Rank(move.to);
	if (move.promotion & pi_color)
	{
		r[4]= english_chars[(move.promotion & pi_type) - 1];
		r[5]= '\0';
	}
	else
		r[4]= '\0';
	return r;
}

