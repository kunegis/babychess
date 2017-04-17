#include "raw_pgn.hh"

#include "time.hh"

void Raw_PGN::Normalize()
{
	if (! Tag("Event").Is())    tags.Set("Event", "?");
	if (! Tag("Site").Is())     tags.Set("Site", "?");
	if (! Tag("Date").Is())     tags.Set("Date", "?");
	if (! Tag("Round").Is())    tags.Set("Round", "?");
	if (! Tag("White").Is())    tags.Set("White", "?");
	if (! Tag("Black").Is())    tags.Set("Black", "?");
	if (! Tag("Result").Is())   tags.Set("Result", "*"); 
}

void Raw_PGN::Reset()
{
	/* Keep parallel to Game_Info::Reset().  */

	tags.Free();
	moves.Free();

	struct timeval now= Now();

	Verbose_Int((int) now.tv_sec);
	String time_text= Format_Time(now, "%Y.%m.%d");

	Verbose_Str(time_text());

	tags.Set("Date",  time_text);
	/* the first move number is contained in tags["FEN"]. */ 
}
