#ifndef GAME_ARCHIVE_HH
#define GAME_ARCHIVE_HH

/* Archive of chess games */

#include "vector.hh"
#include "game.hh"
#include "pointer.hh"
#include "raw_pgn.hh"

class Game_Archive
	:  public Pointed
{
public:

	bool Set_Size(int n);

	void Clear()
		{
			games.Free();
			raw.Free();
		}

	int operator * () const { return *games;  }

	Game &operator [] (int n) 
		{
			assert (games[n].Is());
			return games[n]();
		}

	const Game &operator [] (int n) const
		{
			assert (games[n].Is());
			return games[n]();
		}

	Raw_PGN &Get_Raw(int n) {  return raw[n];  }
	const Raw_PGN &Get_Raw(int n) const { return raw[n];  }
	void Invalidate_Game(int n)  {  games[n].Free();  }

	String Get_White(int i) const
		{
			if (games[i].Is())
				return games[i]().info.players[cc_w];
			String result= raw[i].Tag("White");
			if (result == "?")
				result= "";
			return result;
		}
	String Get_Black(int i) const
		{
			if (games[i].Is())
				return games[i]().info.players[cc_b];
			String result= raw[i].Tag("Black");
			if (result == "?") result= "";
			return result;
		}
	String Get_Event(int i) const
		{
			if (games[i].Is())
				return games[i]().info.event;
			String result= raw[i].Tag("Event");
			if (result == "?")  result= "";
			return result;
		}
	String Get_Round(int i) const
		{
			if (games[i].Is())
				return games[i]().info.round;
			String result= raw[i].Tag("Round");
			if (result == "?")  result= "";
			return result;
		}
	String Get_Date_Locally(int i) const;
	void Get_Date(int i, int &year, int &month, int &day) const
		{
			if (games[i].Is())
			{
				year= games[i]().info.date_year;
				month= games[i]().info.date_month;
				day= games[i]().info.date_day;
			}
			else 
				Game_Info::Parse_Date(raw[i].Tag("Date"), year, month, day);
		}
	String Get_Site(int i) const
		{
			if (games[i].Is())
				return games[i]().info.site;
			String result= raw[i].Tag("Site");
			if (result == "?")  result= "";
			return result;
		}

	bool Check(int n);
	/* Return TRUE if game N exists. 
	 */
	bool Add_Game(int index);
	void Remove(int n);
	void Set(int n,
		 const Game &game);
	bool Have_Game(int n) const
		{
			assert (n >= 0 && n < *raw);
			return games[n].Is();
		}
	int Get_Result(int index); 
	String Get_Readable_Result(int index);

	bool Transfer_From(Game_Archive &ga);
	/* Append all games in GA to THIS.  Games may be lost on
	 * error.  */
	int Compare(int a, int b, int crit);

private:
	Vector <Raw_PGN> raw;
	Vector <Dynamic <Game> > games;
	/* RAW and GAMES have the same size.  If GAMES[n] == NULL,
	 * then RAW[n] contains the game, else GAMES[n]. 
	 */

};

#endif /* ! GAME_ARCHIVE_HH */
