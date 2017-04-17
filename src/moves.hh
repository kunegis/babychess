#ifndef MOVES_HH
#define MOVES_HH

/* Tables for generating legal moves.  Used in LEGAL and POSITION. 
 */

#include "square.hh"
#include "piece.hh"

extern const int moves_offset[16];
/* Numbers that must be added to a square to make a move. 
 *
 *    0 ...  3:  move one square on files and rows
 *    4 ...  7:  move one square on diagonals
 *    8 ... 15:  knight moves
 */

extern const int moves_range[2][3];
/* Which offsets from moves_offset to consider.
 */

#endif /* ! MOVES_HH */ 
