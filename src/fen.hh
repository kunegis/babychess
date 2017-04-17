#ifndef FEN_HH
#define FEN_HH

/* A notation that describes chess positions. 
 *
 * Forsythe-Edwards Notation, as defined by the PGN standard.
 *
 * See <http://www.clark.net/pub/pribut/standard.txt>. 
 */

#include "position.hh"
#include "string.hh"

String FEN(const Position &position);
/* Return the FEN description of POSITION. 
 */

bool Parse_FEN(String       text,
	       Position    &position);
/* Parse the FEN string TEXT, and set POSITION to the new position.
 * On error, return FALSE and set the error in the ERROR module.  On
 * success, the position is put in legal mode, else it is put in not legal
 * mode. 
 */

#endif /* ! FEN_HH */
