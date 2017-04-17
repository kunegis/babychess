#ifndef MOVE_SCORE_HH
#define MOVE_SCORE_HH

#include "move.hh"
#include "material.hh"

int Move_Score(const Move &move,
	       int piece,
	       int game_type);
	/* How MOVE changes the position.  PIECE is the moving piece.
	 * COUNT is the piece count before the move. 
	 */

#endif /* ! MOVE_SCORE_HH */ 
