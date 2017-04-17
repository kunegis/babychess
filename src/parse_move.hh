#ifndef PARSE_MOVE_HH
#define PARSE_MOVE_HH

#include "generator.hh"

int Parse_Move(String           move_text,
	       const Generator &generator);
/* Find the move in GENERATOR that is the best match for MOVE_TEXT.
 * Moves in the standard algebraic notation are always found.  Other
 * notations may be recognized too.  
 *
 * The return value is the index of the move in GENERATOR, or <0 if no matching
 * move was found.   
 */

#endif /* ! PARSE_MOVE_HH */
