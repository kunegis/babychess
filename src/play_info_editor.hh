#ifndef PLAY_INFO_EDITOR_HH
#define PLAY_INFO_EDITOR_HH

#include "play_info.hh"
#include "object_editor.hh"
#include "player_widget.hh"
#include "time_widget.hh"
#include "position_widget.hh"

class Play_Info_Editor
	:  public Object_Editor <Play_Info>
{
public:
	const char *EW_Type_Name() const  { return _("Choose Players");    }

private:
	Player_Widget player_widgets[2];
	Bevel bevel, bevel_1; 
	Time_Widget time_widget;
	Position_Widget position_widget; 
	Text_Button swap_button;

	void OE_Update(const Play_Info &);
	bool OE_Open(Pointer <Widget> parent, const Play_Info &); 
	bool OE_Retrieve(Play_Info &);
	bool OE_Hide_Apply() const { return true; }
	const char *OE_OK_Text() const { return _("_Play"); }
};

#endif /* ! PLAY_INFO_EDITOR_HH */ 
