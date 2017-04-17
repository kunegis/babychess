#ifndef FILENAME_HH
#define FILENAME_HH

/* Operations on filenames */

#include "string.hh"

String operator / (String dir, String file);
/* Return DIR "/" FILE.  */ 

#endif /* ! FILENAME_HH */
