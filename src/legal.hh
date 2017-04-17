#ifndef LEGAL_HH
#define LEGAL_HH

/* Generate legal moves */

/* Definition:  Pseudo-legal moves are the legal moves plus those that
 * put (or keep) the own king in check.   
 */

#include "position.hh"
#include "vector.hh"

class Legal
{
public:
	enum {max_leg= 262};
	/* Maximum possible number of moves in a position. 
	 */

	/* flags */ 
	enum {gn_promotion= 1 << 0};

	static void Generate(Position &position, 
			     Vector <Move> &moves,
			     bool strict_legal,
			     bool just_check);
	/* Generate legal moves in POSITION.
	 *
	 * Append moves to the array MOVES.
	 *
	 * If STRICT_LEGAL is TRUE, only legal moves are generated.  If it is
	 * false, "pseudo-legal" moves are also generated.
	 *
	 * If JUST_CHECK is TRUE, just checks *whether* there are
	 * (pseudo-)legal moves, but don't generate any.  MOVES is still used
	 * in this case.  There is at least one (legal) move if the size of
	 * MOVES is non-zero. 
	 */
	
	static int flags; 
	/* Set by Generate().  Contains the gn_* flags. 
	 */

private:
	static int to_move;
	/* side to move */
	static int move_index;
	/* Index where the next move will be written in GEN_LEG_MOVES. 
	 */
	static bool just_check;
	static bool strict_legal;
	/* Whether checking for only legal moves. 
	 */
	static Pointer <Position> sposition;
	static Pointer <Vector <Move> > smoves;
	static int square;
	/* Current square.  */

	static bool Append(int to, 
			   int captured_piece,
			   int promotion_piece= pi_empty, 
			   int ep_capture= 0, 
			   int castling= 0);
	/* Return value:  whether generation should terminate. 
	 */
};

#endif /* ! LEGAL_HH */
