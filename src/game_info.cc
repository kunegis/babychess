#include "game_info.hh"

#include "format.hh"
#include "piece.hh"
#include "time.hh"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

const char *Game_Info::result_text[4]=
{"1-0",
 "1/2-1/2",
 "0-1",
 "*"};
const char *Game_Info::result_readable_text[4]=
{"1–0", /* a UTF-8 long dash */
 "½–½",
 "0–1", /* a UTF-8 long dash */ 
 "*"};

void Game_Info::Set(const Hash_Table &all_tags)
{
	tags= all_tags; 

	tags.Remove("FEN"); 
	tags.Remove("SetUp"); 

	event= tags.Get("Event");
	tags.Remove("Event");
	if (event == '?')
		event= "";

	site= tags.Get("Site");
	tags.Remove("Site");
	if (site == '?')
		site= "";

	String date= tags.Get("Date");
	tags.Remove("Date");

	Parse_Date(date, date_year, date_month, date_day);

	round= tags.Get("Round");
	tags.Remove("Round");
	if (round == '?')
		round= "";

	players[cc_w]= tags.Get("White");
	tags.Remove("White");
	if (players[cc_w] == "?")
		players[cc_w]= ""; 

	players[cc_b]= tags.Get("Black");
	tags.Remove("Black");
	if (players[cc_b] == "?")
		players[cc_b]= "";  
			
	String new_result= tags.Get("Result");
	tags.Remove("Result");

	result= Decode_Result(new_result);
}

void Game_Info::Reset()
{
	/* Maintain parallel to RAW_PGN::Reset();  */

	event= site= round= players[cc_w]= players[cc_b]= ""; 
	result= re_unfinished; 
	tags.Free(); 
	first_move_number= 1;

	struct timeval now= Now();
	struct tm *tm= localtime(&now.tv_sec);
	if (tm)
	{
		date_year=  1900 + tm->tm_year;
		date_month= tm->tm_mon;
		date_day=   tm->tm_mday;
	}
	else
		date_year= date_month= date_day= -1; 
}

String Game_Info::Format_Date() const
{
	char buf[]= "????.??.??";

	if (date_year >= 0)
	{
		int year= date_year;
		buf[3]= '0' + year % 10;
		year /= 10;
		buf[2]= '0' + year % 10;
		year /= 10;
		buf[1]= '0' + year % 10;
		year /= 10;
		buf[0]= '0' + year % 10;
	}

	if (date_month >= 0)
	{
		int month= date_month + 1;
		buf[6]= '0' + month % 10;
		buf[5]= '0' + month / 10 % 10;
	}

	if (date_day >= 0)
	{
		buf[9]= '0' + date_day % 10;
		buf[8]= '0' + date_day / 10 % 10;
	}

	return buf; 
}

String Game_Info::Format_Date_Locally(int year, int month, int day) 
{
	if (year < 0)   return "";

	if (month < 0)  return Format("%d", year);
	
	if (day < 0)  day= 1;

	String date_F_text= Format("%d-%d-%d", year, month + 1, day);

	Verbose_Str(date_F_text());

	struct timeval date= ::Parse_Time(date_F_text(), "%Y-%m-%d");
	/* %F is not supported on Solaris; it's a GNU extension. */

	Verbose_Int((int) date.tv_sec);
	
	String ret= ::Format_Time(date, "%x");

	Verbose_Str(ret());

	return ret;
}

void Game_Info::Parse_Date(String date, int &year, int &month, int &day)
{
	if ((! date.Is()) || date[0] == '?')
	{
		year= month= day= -1;
	}
	else
	{
		int s= sscanf(date(),
			      "%d.%d.%d",
			      & year,
			      & month,
			      & day);

		switch (s)
		{
		default:  
		case 0:   year= -1;
		case 1:   month= 0;
		case 2:   day= -1;
		case 3:   -- month;
		}
	}
}

int Game_Info::Decode_Result(String result)
{
	if (result == "1-0")
		return re_white;
	else if (result == "0-1")
		return re_black;
	else if (result == "1/2"  ||
		 result == "1/2-1/2")
		return re_draw;
	return re_unfinished; 
}
