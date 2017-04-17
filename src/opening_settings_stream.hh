#ifndef OPENING_SETTINGS_STREAM_HH
#define OPENING_SETTINGS_STREAM_HH

#include "opening_settings.hh"

#include <stdio.h>

bool Read_Stream(FILE *file, Opening_Settings *settings);
bool Write_Stream(FILE *file, const Opening_Settings *settings);

#endif /* ! OPENING_SETTINGS_STREAM_HH */ 
