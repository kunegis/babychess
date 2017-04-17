#ifndef SQUARE_VALUE_HH
#define SQUARE_VALUE_HH

#include "inline.hh"
#include "square.hh"

enum 
{
	sv_wP, 
	sv_bP,
	sv_wK,
	sv_bK,
	sv_g_c,  /* great center  */ 
	sv_s_c,  /* square center */ 
	sv_d_c,  /* diagonal center  */ 
	sv_dd_c, /* double diagonal center */ 
	sv_ds_c, /* double square center */ 
	sv_wrace,/* pawn race */ 
	sv_brace,

	sv_count
};

extern const int square_value[sv_count][120]; 

Inline int Square_Value(int square,
			int type)
{
	assert (Is_Inside_Square(square));
	assert (type >= 0 && type < sv_count);

	return square_value[type][square]; 
}

#endif /* ! SQUARE_VALUE_HH */
