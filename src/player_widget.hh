#ifndef PLAYER_WIDGET_HH
#define PLAYER_WIDGET_HH

#include "radio_group.hh"
#include "player.hh"
#include "piece_widget.hh"
#include "engine_menu.hh"

class Player_Widget
	:  public V_Box
{
public:
	bool Open(Pointer <Widget> parent, 
		  const Player &player,
		  int color);

	void Set(const Player &);
	void Retrieve(Player &);

private:
	H_Box type_box;
	V_Box color_box;
	Label color_label; 
	Piece_Widget piece; 
	Radio_Group type;
	Engine_Menu engine_menu;
	Label blank; 
	Labeled_Entry name;

	void MR_Receive(String);
};

#endif /* ! PLAYER_WIDGET_HH */
