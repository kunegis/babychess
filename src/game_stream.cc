#include "game_stream.hh"

#include "pgn.hh"
#include "error.hh"
#include "gettext.hh"

bool Read_Stream(FILE *file, 
		 Game *game) 
{
	String read_ahead;
	if (! PGN::Parse(file, *game, read_ahead))
		return false;

	Game second_game;
	if (PGN::Parse(file, second_game, read_ahead))
	{
		Error::Set(_("Archive contains more than one game"));
		return false;
	}
	return true;
}

bool Write_Stream(FILE *file, 
		  const Game *game)
{
	return PGN::Write(file, *game);
}

bool Check_Filename(String filename, Game *)
{
	return *filename >= 4 && 0 == memcmp(filename()+*filename-4, ".pgn", 4);
}

