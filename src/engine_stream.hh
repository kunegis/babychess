#ifndef ENGINE_STREAM_HH
#define ENGINE_STREAM_HH

#include "engine.hh"

#include <stdio.h>

bool Read_Stream(FILE *, Engine *engine);
bool Write_Stream(FILE *, const Engine *engine);

#endif /* ! ENGINE_STREAM_HH */
