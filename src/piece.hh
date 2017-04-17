#ifndef PIECE_HH
#define PIECE_HH

#include "inline.hh"

/* In addition to the 12 pieces, a piece can be an empty square or a
 * square outside the board.
 */

/* Pieces are coded as integers and fit in a 7 bit integer:
 *
 * Values:
 *
 *    16..21:    white pieces  
 *    32..37:    black pieces
 *    8:         empty square  
 *    64:        outside
 *
 * Bits:
 *
 *    0..2:     piece type    > zero if not a piece (also 0 for pawn)
 *    3:        empty         \
 *    4:        white         | exactly one of those four is set
 *    5:        black         |
 *    6:        outside       /
 */

enum {
	pi_P=          0, /* pawn   */   
	pi_N,             /* knight */
	pi_B,             /* bishop */
	pi_R,             /* rook   */
	pi_Q,             /* queen  */
	pi_K,             /* king   */
	pi_count,

	pi_type=       7, /* mask to get only the type */ 

	pi_empty=      8, 

	/* OR with a piece type to get a full piece */ 
	pi_white=     16,
	pi_black=     32,

	/* To get only the color or to test whether the piece is not
	 * empty and not outside */ 
	pi_color=     pi_white | pi_black,


	pi_outside=   64,

	/* filters out the color, but leaves the outside/empty bits.  Use like:  
	 * if ((piece & pi_istype) == pi_N)
	 */
	pi_istype=    79,

	pi_nocolor=   72, /* empty or outside */ 

	/* pi_can_COLOR:  whether a piece of COLOR can move to the square.
	 * Example:  (piece & pi_can_white) is true if the piece is black or
	 * empty. */
	pi_can_white= pi_empty | pi_black, 
	pi_can_black= pi_empty | pi_white,

	/* all pieces */ 
	pi_wP=      16,  pi_bP=      32,
	pi_wN=      17,  pi_bN=      33,
	pi_wB=      18,  pi_bB=      34,
	pi_wR=      19,  pi_bR=      35,
	pi_wQ=      20,  pi_bQ=      36,
	pi_wK=      21,  pi_bK=      37,

	/* piece or square color */ 
	cc_w= 0, /* white */
	cc_b= 1  /* black */
	/* Note:  to have opposite color, use 1^color.    */
	/* To get the type of a piece:  piece & pi_type   */
};

Inline int Piece(int cc, int pt)
{
	return (16 << cc) | pt;
}

/* Extract the color (0 = white, 1 = black) */
Inline int Piece_Color(int piece)
{
	return (piece & pi_black) >> 5;
}

/* Extract the opposite color */ 
Inline int Piece_NColor(int piece)
{
	return (piece & pi_white) >> 4;
}

Inline int Is_Piece_Of_Color(int piece, int cc)
{
	return (16 << cc) & piece;
}

Inline int Is_Piece_Of_NColor(int piece, int cc)
{
	return (32 >> cc) & piece; 
}

#endif /* ! PIECE_HH */
