#ifndef EXECUTE_HH
#define EXECUTE_HH

/* Execute a program in the background. 
 */

#include "string.hh"
#include "vector.hh"

bool Execute(const char *program_filename,
//	     Pointer <const Vector <String> > args= NULL);
	     const char *const args[]);

#endif /* ! EXECUTE_HH */
