#include "move.hh"

bool Move::operator==(const Move &move) const
{
	assert (0);
	/* Should use this as another function, and memcmp() for
	 * normal comparison. 
	 */

	/* do not compare all fields */
	return
		from == move.from &&
		to == move.to &&
		promotion == move.promotion; 
}

