#ifndef PRINTF_SEP_HH
#define PRINTF_SEP_HH

/* The string to insert between % and d in printf() calls to obtain separation
 * of thousands according to the locale.  E.g. printf("%"PRINTF_SEP"d", 1234)
 * would produce "1.234" in Germany and "1 234" in France. 
 */

#include "../config.hh"

#if HAVE_PRINTF_SEP
#  define  PRINTF_SEP "'"
#else 
#  define  PRINTF_SEP
#endif

#endif /* ! PRINTF_SEP_HH */ 
