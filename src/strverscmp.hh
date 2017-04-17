#ifndef STRVERSCMP_HH
#define STRVERSCMP_HH

#include "../config.hh"

#if HAVE_STRVERSCMP

#   include <string.h>

#else /* ! HAVE_STVERSCMP */ 

#   include  <string.h>
#   define strverscmp strcmp

#endif /* ! HAVE_STRVERSCMP */ 

#endif /* ! STRVERSCMP_HH */ 
