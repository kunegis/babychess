#ifndef GAME_ARCHIVE_STREAM_HH
#define GAME_ARCHIVE_STREAM_HH

#include "game_archive.hh"

#include <stdio.h>

bool Read_Stream(FILE *, Game_Archive *);
bool Write_Stream(FILE *, const Game_Archive *);

#endif /* ! GAME_ARCHIVE_STREAM_HH */
