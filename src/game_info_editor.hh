#ifndef GAME_INFO_EDITOR_HH
#define GAME_INFO_EDITOR_HH

#include "object_editor.hh"
#include "box.hh"
#include "entry.hh"
#include "game_info.hh"
#include "label.hh"
#include "spin_button.hh"
#include "radio_group.hh"
#include "gettext.hh"
#include "title_box.hh"

class Game_Info_Editor
	:  public Object_Editor <Game_Info>
{
public:
	const char *EW_Type_Name() const  
		{  return _("Game Properties");   }
	void Set_Result(int);

private:
	Title_Box entry_box;
//	Title_Label label;
	Table box; 
	Label labels[7]; 
	Entry entries[6]; 
	Framed_Radio_Group result_chooser; 
	Spin_Button first_move_button; 

	bool OE_Open(Pointer <Widget> parent, const Game_Info &); 
	bool OE_Retrieve(Game_Info &);
};

#endif /* ! GAME_INFO_EDITOR_HH */ 
