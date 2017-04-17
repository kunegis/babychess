#ifndef MAIN_HH
#define MAIN_HH

/* This module implements the main() function. 
 *
 * The main() function will call the function Main() which must 
 * exist. 
 */

#include "../config.hh"
#include "string.hh"


#if ! HAVE_PROGRAM_INVOCATION_NAME
   extern const char *program_invocation_name, *program_invocation_short_name; 
   /* Set at the beginning of main(). */
#else
   /* declares program_invocation_name */ 
#    include <errno.h>
#endif


int Main(int argc, char **argv);
/* The Main() function must be defined by the program and is called
 * from the MAIN module. 
 *
 * ARGC and ARGV are the arguments of the usual main() function. 
 */

#endif /* ! MAIN_HH */

