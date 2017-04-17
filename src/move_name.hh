#ifndef MOVE_NAME_HH
#define MOVE_NAME_HH

/* The name of a chess move.  All information necessary to generate to textual
 * representation of a move. 
 */

#include "square.hh"

struct Move_Name
{
public:
	unsigned piece_type      :3; 
	unsigned capture         :3; 
	/* Piece type of captured piece + 1 or 0 when not a capture 
	 */

	unsigned from_file       :3;
	unsigned from_rank       :3;
	unsigned to_file         :3;
	unsigned to_rank         :3;

	unsigned special         :3; 
	/* 0: normal move
	 * 1: en passant
	 * 2: O-O
	 * 3: O-O-O
	 * 4..7: promotion piece type + 3 
	 */

	unsigned precision       :2;
	/* Additional information needed in the short notation for
	 * ambigous moves. 
	 *
	 *   0:  no additional information needed    Nd7
	 *   1:  show destination file               Nbd7
	 *   2:  show destination rank               R5e7
	 *   3:  show both                           Qa7b8
	 */

	unsigned check           :2;
	/*   0:  no check
	 *   1:  check
	 *   2:  double check
	 *
	 * The move may additionally be mate or stalemate, as
	 * indicated by FORCE. 
	 */

	unsigned force           :1;  /* Wether the move is mate/stalemate */
	unsigned color           :1;  /* who does the move */ 
};

#endif /* ! MOVE_NAME_HH */ 
