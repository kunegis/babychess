#ifndef MOVE_HH
#define MOVE_HH

/* Information necessary to play the move on the board, and to take it back 
 */

#include "square.hh"
#include "piece.hh"
#include "inttypes.hh"
#include "pointer.hh"

enum {wO_O=   4, 
      wO_O_O= 5,
      bO_O=   6,
      bO_O_O= 7}; 

typedef uint32_t move_t; 
/* in the so-called "small" format */ 

struct Move
	:  public Pointed
{
	int from, to;
	/* squares.  If the move is a castling, the squares of the
	 * king move  */

	int piece;
	/* piece that does the move */

	int ep_poss;
	/* Where could there have been an "en passant" capture; for instance
	 * e5  */

	int promotion;
	/* The promotion piece.
	 *   empty square:  no promotion
	 *   piece:         promotion into... (including color)
	 */

	int ep;
	/* non-zero:  this move is an "en passant" capture of this
	 *            square (eg e5)
	 * zero:      the move is not an "en passant" capture
	 */

	int captured;
	/* The piece that is captured.  Empty when the move is not a
	 * capture, else the piece that was captured, including the
	 * color.  A pawn also for en passant moves.
	 */

	int castling;
	/* castling or 0 */ 

	int halfmove_clock;
	/* Halfmove clock of the position before the move. 
	 */
	bool check;
	/* Whether the position was check before the move.  */

	Move() {}
	Move(move_t small_move,
	     int      new_ep_poss,
	     int      new_halfmove_clock,
	     int      new_piece,
	     int      new_check)
		:  from           ((small_move >>  0) & 0xFF),
		   to             ((small_move >>  8) & 0xFF),
		   piece          (new_piece),
		   ep_poss        (new_ep_poss),
		   promotion      ((small_move >> 24) &  0xF),
		   ep             ((small_move >> 16) & 0xFF),
		   captured       ((small_move >> 28) &  0xF),
		   halfmove_clock (new_halfmove_clock),
		   check          (new_check)
		{
			if (! (promotion & pi_empty))   
				promotion |= (new_piece & pi_color);
			if (! (captured  & pi_empty))   
				captured  |= ((new_piece & pi_color) ^ pi_color);
			if ((piece & pi_type) == pi_K && File(from) == sq_e)
			{
				if (File(to) == sq_g)
					castling= 4 | ((piece & pi_black) >> 4);
				else if (File(to) == sq_c)
					castling= 5 | ((piece & pi_black) >> 4);
				else
					castling= 0;
			}
			else
				castling= 0;
		}

	bool operator == (const Move &move) const;
	/* deprecated; see comment in implementation. */ 
	bool Same_As(const Move &move) const
	/* The two moves to compare must be legal in the same
	 * position.  This function exists because it is not necessary
	 * to compare all fields. 
	 */
		{
			return
				from == move.from &&
				to == move.to     &&
				promotion == move.promotion; 
		}

	int Captured_Square() const
		/* The move must be a capture.  Return the square that was captured.  Equal
		 * to TO except for "en passant" moves.  */
		{ return ep ?: to;  }

	int Rook_From() const
		/* THIS must be a castling.  Return the FROM square of the rook
		 * sub-move. */ 
		{
			assert (castling);
			return Make_Square(File(to) == sq_g ? sq_h : sq_a,
					   Rank(to));
		}
  
	int Rook_To() const
		/* TO equivalent of Rook_From(). */
		{
			assert (castling);
			return Make_Square(File(to) == sq_g ? sq_f : sq_d, Rank(to));
		}

	/* Put all necessary move info into 32 bits:
	 * Bits:  0..7:    from   
	 *        8..15:   to
	 *        16..23:  ep 
	 *        24..27:  promotion_piece & 0xF (i.e., drop color info
	 *          	                          but keep empty)
	 *        28..31:  captured_piece  & 0xF (")
	 */
	move_t Small() const
		{
			return 
				(from <<  0) | 
				(to   <<  8) |
				(ep   << 16) |
				((promotion & (pi_type | pi_empty)) << 24) |
				((captured  & (pi_type | pi_empty)) << 28);
		}

	static int Promotion_Piece_Index(int piece)
		{
			return piece & pi_empty ? 0 : 4 - piece;
		}
};

#endif /* ! MOVE_HH */
