#ifndef GAME_STREAM_HH
#define GAME_STREAM_HH

#include "game.hh"

#include <stdio.h>

bool Read_Stream(FILE *, Game *game);
bool Write_Stream(FILE *, const Game *game); 
bool Check_Filename(String, Game *);

#endif /* ! GAME_STREAM_HH */
