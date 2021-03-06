#include "square_value.hh"

/* The board is upside down. */

enum {sv_base= 3}; 

#define ______________________\
    0,0,0,0,0,0,0,0,0,0,\
    0,0,0,0,0,0,0,0,0,0,

#define f - 6 * sv_base
#define e - 5 * sv_base
#define d - 4 * sv_base
#define c - 3 * sv_base
#define b - 2 * sv_base
#define a - 1 * sv_base
#define A   1 * sv_base
#define B   2 * sv_base
#define C   3 * sv_base
#define D   4 * sv_base
#define E   5 * sv_base
#define F   6 * sv_base

const int square_value[sv_count][120]=
{            
	{ /* wP */
		______________________
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,A,A,0,0,0, 0, 
		0, 0,0,A,C,C,A,0,0, 0, 
		0, 0,A,A,C,C,A,A,0, 0, 
		0, B,B,B,C,C,B,B,B, 0, 
		0, B,B,B,B,B,B,B,B, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		______________________
	},
	{ /* bP */
		______________________
		0, 0,0,0,0,0,0,0,0, 0, 
		0, B,B,B,B,B,B,B,B, 0, 
		0, B,B,B,C,C,B,B,B, 0, 
		0, 0,A,A,C,C,A,A,0, 0, 
		0, 0,0,A,C,C,A,0,0, 0, 
		0, 0,0,0,A,A,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		______________________
	},
	{ /* wK   */
		______________________
		0, B,B,0,0,0,0,B,B, 0, 
		0, A,A,0,0,0,0,A,A, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		______________________
	},
	{ /* bK   */
		______________________
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, A,A,0,0,0,0,A,A, 0, 
		0, B,B,0,0,0,0,B,B, 0, 
		______________________
	},
	{ /* great center   */
		______________________
		0, a,a,a,a,a,a,a,a, 0, 
		0, a,0,0,0,0,0,0,a, 0, 
		0, a,0,B,B,B,B,0,a, 0, 
		0, a,0,B,B,B,B,0,a, 0, 
		0, a,0,B,B,B,B,0,a, 0, 
		0, a,0,B,B,B,B,0,a, 0, 
		0, a,0,0,0,0,0,0,a, 0, 
		0, a,a,a,a,a,a,a,a, 0, 
		______________________
	},
	{ /* square center   */
		______________________
		0, b,b,b,b,b,b,b,b, 0, 
		0, b,a,a,a,a,a,a,b, 0, 
		0, b,a,0,0,0,0,a,b, 0, 
		0, b,a,0,A,A,0,a,b, 0, 
		0, b,a,0,A,A,0,a,b, 0, 
		0, b,a,0,0,0,0,a,b, 0, 
		0, b,a,a,a,a,a,a,b, 0, 
		0, b,b,b,b,b,b,b,b, 0, 
		______________________
	},
	{ /* diagonal center   */
		______________________
		0, f,e,d,c,c,d,e,f, 0, 
		0, e,d,c,b,b,c,d,e, 0, 
		0, d,c,b,a,a,b,c,d, 0, 
		0, c,b,a,0,0,a,b,c, 0, 
		0, c,b,a,0,0,a,b,c, 0, 
		0, d,c,b,a,a,b,c,d, 0, 
		0, e,d,c,b,b,c,d,e, 0, 
		0, f,e,d,c,c,d,e,f, 0, 
		______________________
	},
	{ /* double diagonal center   */
		______________________
		0, f,d,b,0,0,b,d,f, 0, 
		0, d,b,0,B,B,0,b,d, 0, 
		0, b,0,B,D,D,B,0,b, 0, 
		0, 0,B,D,F,F,D,B,0, 0, 
		0, 0,B,D,F,F,D,B,0, 0, 
		0, b,0,B,D,D,B,0,b, 0, 
		0, d,b,0,B,B,0,b,d, 0, 
		0, f,d,b,0,0,b,d,f, 0, 
		______________________
	},
	{ /* double square center   */
		______________________
		0, d,d,d,d,d,d,d,d, 0, 
		0, d,b,b,b,b,b,b,d, 0, 
		0, d,b,0,0,0,0,b,d, 0, 
		0, d,b,0,B,B,0,b,d, 0, 
		0, d,b,0,B,B,0,b,d, 0, 
		0, d,b,0,0,0,0,b,d, 0, 
		0, d,b,b,b,b,b,b,d, 0, 
		0, d,d,d,d,d,d,d,d, 0, 
		______________________
	},
	{ /* white race   */
		______________________
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, A,A,A,A,A,A,A,A, 0, 
		0, B,B,B,B,B,B,B,B, 0, 
		0, C,C,C,C,C,C,C,C, 0, 
		0, D,D,D,D,D,D,D,D, 0, 
		0, E,E,E,E,E,E,E,E, 0, 
		0, F,F,F,F,F,F,F,F, 0, 
		______________________
	},
	{ /* black race   */
		______________________
		0, F,F,F,F,F,F,F,F, 0, 
		0, E,E,E,E,E,E,E,E, 0, 
		0, D,D,D,D,D,D,D,D, 0, 
		0, C,C,C,C,C,C,C,C, 0, 
		0, B,B,B,B,B,B,B,B, 0, 
		0, A,A,A,A,A,A,A,A, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		0, 0,0,0,0,0,0,0,0, 0, 
		______________________
	},
};

#undef ______________________
#undef f
#undef e
#undef d
#undef c
#undef b
#undef a
#undef A
#undef B
#undef C
#undef D
#undef E
#undef F
