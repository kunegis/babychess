#ifndef PIECE_NAME_HH
#define PIECE_NAME_HH

#include "string.hh"
#include "inline.hh"
#include "gettext.hh"

extern const char *const piece_name_names[6];

Inline const char *Piece_Name(int pt)
	/* Name of the piece type, translated.  */
{
	assert (pt >= 0 && pt < 6);

	return _(piece_name_names[pt]);
}

#endif /* ! PIECE_NAME_HH */
