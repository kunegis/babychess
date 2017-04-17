#ifndef READ_LINE_HH
#define READ_LINE_HH

/* In most cases, Line_Reader should be used instead of this. 
 */

#include "string.hh"

#include <stdio.h>

bool Read_Line(FILE *,
	       String &line,
	       bool *end_of_line= NULL);
/* Read a line from FILE.  Write the line read into LINE.  Remove
 * terminating newline.  Return FALSE on error or end-of-file.  The
 * resulting LINE may be empty. 
 * END_OF_LINE:  Set to whether to line ended (only meaningful when not
 *  	         blocking)  
 */

#endif /* ! READ_LINE_HH */
