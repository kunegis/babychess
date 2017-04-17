#include "game_archive_stream.hh"

#include "pgn.hh"

bool Read_Stream(FILE *file,
		 Game_Archive *game_archive)
{
	game_archive->Clear();
	String read_ahead;
	while (true)
	{
		int size= **game_archive;
		if (! game_archive->Set_Size(size + 1))
			return false;
		if (! PGN::Parse_Raw(file,
				     game_archive->Get_Raw(size),
				     read_ahead))
		{
			game_archive->Set_Size(size);
			return true;
		}
		game_archive->Invalidate_Game(size);
	}
}

bool Write_Stream(FILE *file,
		  const Game_Archive *game_archive)
{
	for (int i= 0;  i < **game_archive;  ++i)
	{
		if (game_archive->Have_Game(i))
		{
			if (! PGN::Write(file, (*game_archive)[i]))
				return false;
		}
		else
		{
			if (! PGN::Write_Raw(file,
					     game_archive->Get_Raw(i)))
				return false;
		}
	}
	return true;
}
