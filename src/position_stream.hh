#ifndef POSITION_STREAM_HH
#define POSITION_STREAM_HH

#include "position.hh"

#include <stdio.h>

bool Read_Stream(FILE *, Position *);
bool Write_Stream(FILE *, const Position *);

#endif /* ! POSITION_STREAM_HH */
