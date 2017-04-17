#ifndef GAME_LIST_HH
#define GAME_LIST_HH

#include "list.hh"
#include "game.hh"

class Game_List
	:  public List
{
public:
	bool Open(Pointer <Widget> parent,
		  const Game &,
		  const Notation &,
		  const Notation_Settings &,
		  const char *clicked);
	int Location() const 
		{
			assert (location >= 0); 
			return location; 
		}
	void Update_Content_One(int location);
	void Update_Length();
	void Update_Content(int begin= 0, int end= -1);
	/* if END=-1, update to the end,  Use (0, -1) to update all.  
	 */
	void Update_Highlight();
	void Update()
		{
			Update_Length();
			Update_Content(); 
			Update_Highlight(); 
		}

private:
	Pointer <const Game> game;
	Pointer <const Notation> notation; 
	Pointer <const Notation_Settings> notation_settings; 
	String msg;
	int location; 
	int move_list_length;
	/* number of half moves currently shown in game move list.  -1
	 * if nothing is shown.  Used by Update_Move_List_Length(). 
	 */
	int move_list_first;

	void MR_Receive(String);

	void Wdg_Close();
};

#endif /* ! GAME_LIST_HH */ 

