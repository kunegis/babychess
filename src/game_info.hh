#ifndef GAME_INFO_HH
#define GAME_INFO_HH

#include "hash_table.hh"

enum {re_white= 0, re_draw, re_black, re_unfinished}; 

class Game_Info
	:  public Pointed
{
public:

	String event, site, round;
	String players[2];

	int result; 
	/* The game result is independent from the game moves.
	 * Example:  The game might not end in a checkmate, and the
	 * result might be "1-0".  */

	int date_year, date_month, date_day; 
	/* Month:  0-based. 
	 * Day:    1-based. 
	 * -1 when unknown for each.  (e.g. the year may be known but
	 * not the month/day. 
	 */

	Hash_Table tags;  
	/* Additional tags  */

	/* The FEN tag is not part of Game_Info.  It is part of the
	 * game.  */

	int first_move_number;
	/* Note:  this must be kept in synch with the starting
	 * position.  */

	void Reset();

	void Set(const Hash_Table &all_tags);
	/* ALL_TAGS contains all tags, including those that
	 * are stored as a string and Result in Game_Info.
	 * Ignore "FEN" and "SetUp" tags. 
	 */
	String Get_Result_Text() const
		{ return Get_Result_Text(result);   }
	String Get_Result_Readable_Text() const
		{ return Get_Result_Readable_Text(result); }
	String Format_Date() const;
	/* In the format mandated by the PGN standard */ 

	String Format_Date_Locally() const
	/* Using the locale */
		{
			return Format_Date_Locally(date_year, date_month, date_day); 
		}

	static String Get_Result_Text(int result)
		{
			assert (result >= 0 && result <= 3);
			return result_text[result]; 
		}
	static String Get_Result_Readable_Text(int result)
		{
			assert (result >= 0 && result <= 3);
			return result_readable_text[result]; 
		}
	static void Parse_Date(String date, int &year, int &month, int &day);
	/* DATE is in PGN-format.  Parse it and put the result in the
	 * INTs, as described in the comments of the members of
	 * Game_Info. 
	 */
	static String Format_Date_Locally(int year, int month, int day);
	static int Decode_Result(String result);

private:

	static const char *result_text[4], *result_readable_text[4];
};

#endif /* ! GAME_INFO_HH */ 
