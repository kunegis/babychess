#ifndef NAG_HH
#define NAG_HH

/* Numeric annotation glyph, as defined by the PGN standard.  Absence of a
 * NAG is indicated by 0.  NAGs go from 0 (= no nag) to NAG_COUNT-1.  
 */

#include "string.hh"

enum {nag_count= 46};

int Parse_NAG(String text);
/* Return 0 when no nag matches. 
 */

String NAG(int nag);
/* Return the associated NAG.  Empty string for 0.  $N when too big. 
 */

const char *NAG_Description(int nag);

#endif /* ! NAG_HH */ 
