#include "moveable.hh"

#ifndef NDEBUG

bool (*moveable_hook)(void *, int, const char *)= NULL; 

#endif /* ! NDEBUG */ 
