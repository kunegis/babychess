#include "game_archive.hh"

#include "pgn.hh"
#include "strverscmp.hh"

bool Game_Archive::Set_Size(int n) 
{
	int size= *games;
	if (! games.Set(n))
		return false;
	if (! raw.Set(n))
	{
		games.Set(size);
		return false;
	}
	return true;
}

bool Game_Archive::Check(int n)
{
	if (games[n].Is())
		return true;

	if (! games[n].Alloc())
		return false;

	if (! PGN::Read_Raw(raw[n], games[n]()))
	{
		games[n].Free();
		return false;
	}

	return true;
}

bool Game_Archive::Add_Game(int n)
{
	assert (n >= 0 && n <= *raw);
	assert (*raw == *games); 

	int size= *raw;
	if (! raw.Set(size + 1))
		return false;
	if (! games.Set(size + 1))
	{
		raw.Set(size);
		return false;
	}

	for (int i= n;  i < size;  ++i)
	{
		raw[i + 1]= raw[i];
		games[i].Transfer_To(games[i + 1]);
	}
	/* Let RAW be open on the new game.  */
	raw[n].Reset();
	raw[n].Normalize();

	assert (! games[n].Is());

	return true;
}

void Game_Archive::Remove(int n)
{
	int size= *raw;

	assert (n >= 0 && n < size);

	for (int i= n;  i < size - 1;  ++i)
	{
		raw[i]= raw[i + 1];
		games[i + 1].Transfer_To(games[i]);
	}

	raw.Set(size - 1);
	games.Set(size - 1);
}

void Game_Archive::Set(int n,
		  const Game &game)
{
	assert (n >= 0 && n < *raw);

	games[n].Free();
	if (! games[n].Alloc(game))
	{
		raw[n].Reset();
	}
}

int Game_Archive::Get_Result(int i)
{
	assert (i >= 0 && i < *raw);
	if (games[i].Is())
		return games[i]->info.result;
	else
		return Game_Info::Decode_Result(raw[i].Tag("Result"));
}

String Game_Archive::Get_Readable_Result(int i)
{
	assert (i >= 0 && i < *raw);

	if (games[i].Is())
		return games[i]().info.Get_Result_Readable_Text();

	return Game_Info::Get_Result_Readable_Text
		(Game_Info::Decode_Result(raw[i].Tag("Result"))); 
}

bool Game_Archive::Transfer_From(Game_Archive &a)
{
	assert (*raw == *games);
	assert (*a.raw == *a.games);

	int size= *raw;

	if (! raw.Append_Transfer_From(a.raw, 0))
	{
		return false;
	}

	if (! games.Append_Transfer_From(a.games, 0))
	{
		raw= size;
		a.games= 0;
		return false;
	}

	return true;
}

int Game_Archive::Compare(int a, int b, int crit)
{
	switch (crit)
	{
	default:  assert (0);  return 0;

	case 0:  
	{
		String as= Get_Event(a), bs= Get_Event(b);
		return strcoll(as(), bs());
	}
	case 1:  
	{
		String as= Get_Site(a), bs= Get_Site(b);
		return strcoll(as(), bs());
	}
	case 2:  
	{
		int ayear, amonth, aday, byear, bmonth, bday;
		Get_Date(a, ayear, amonth, aday);
		Get_Date(b, byear, bmonth, bday);
		return ayear != byear ? ayear - byear : amonth != bmonth
			? amonth - bmonth : aday - bday;
	}
	case 3:  
	{
		String as= Get_Round(a), bs= Get_Round(b);
		return strverscmp(as(), bs());
	}
	case 4:  
	{
		String as= Get_White(a), bs= Get_White(b);
		return strcoll(as(), bs());
	}
	case 5:  
	{
		String as= Get_Black(a), bs= Get_Black(b);
		return strcoll(as(), bs());
	}
	case 6:  
	{
		int as= Get_Result(a), bs= Get_Result(b);
		return bs - as; 
	}
	}
}

String Game_Archive::Get_Date_Locally(int i) const
{
	if (games[i].Is())
	{
		return games[i]().info.Format_Date_Locally();
	}
	String result= raw[i].Tag("Date");
	Verbose_Str(result());
	int year, month, day;
	Game_Info::Parse_Date(result, year, month, day);
	Verbose("parsed %d %d %d", year, month, day);
	return Game_Info::Format_Date_Locally(year, month, day); 
}
