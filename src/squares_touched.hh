#ifndef SQUARES_TOUCHED_HH
#define SQUARES_TOUCHED_HH

#include "move.hh"

void Squares_Touched(const Move &, int *squares);
/* Fills the array of size 4 with squares that are changed by
 * this move.  If less than 4 squares are changed, SQUARES[i]
 * is set to zero. 
 */

#endif /* ! SQUARES_TOUCHED_HH */ 
