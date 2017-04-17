#ifndef STRDUPA_HH
#define STRDUPA_HH

#include "../config.hh"

#if ! HAVE_STRDUPA

#include <alloca.h>

#undef  strdupa
#define strdupa(p)                              \
({						\
     int size= strlen(p);			\
     void *r= alloca(size);			\
     memcpy(r, p, size);			\
     (char *)r;                                 \
})


#endif /* ! HAVE_STRDUPA */ 

#endif /* ! STRDUPA_HH */
