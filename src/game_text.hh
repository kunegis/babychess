#ifndef GAME_TEXT_HH
#define GAME_TEXT_HH

#include "book.hh"

#include <stdio.h>

class Game_Text
{
public:
	static bool Write_PGN(FILE *, Book &book);
	static bool Write_HTML(FILE *, Book &book);
};

#endif /* ! GAME_TEXT_HH */
