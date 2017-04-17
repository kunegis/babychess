#include "play_info_editor.hh"

#include "piece.hh"

#define s_swap "swap"

void Play_Info_Editor::OE_Update(const Play_Info &play_info)
{
	player_widgets[cc_w].Set(play_info.players[cc_w]);
	player_widgets[cc_b].Set(play_info.players[cc_b]); 
	time_widget.Set(play_info.time);
	position_widget.Set(play_info.starting_position);
}

bool Play_Info_Editor::OE_Open(Pointer <Widget> parent, 
			     const Play_Info &play_info)
{
	if (! player_widgets[cc_w].Open(parent, play_info.players[cc_w], cc_w))
		return false;
	bevel.Open(parent); 
	if (! player_widgets[cc_b].Open(parent, play_info.players[cc_b], cc_b))
		return false; 
	bevel_1.Open(parent);
	if (! time_widget.Open(parent, play_info.time))
		return false;
	if (! position_widget.Open(parent, play_info.starting_position))
		return false; 
	if (! swap_button.Open(parent, s_swap, _("_Swap Players")))
		return false; 
	return true; 
}

bool Play_Info_Editor::OE_Retrieve(Play_Info &play_info)
{
	player_widgets[cc_w].Retrieve(play_info.players[cc_w]);
	player_widgets[cc_b].Retrieve(play_info.players[cc_b]);
	play_info.time= time_widget.Time();
	play_info.starting_position= position_widget.Get(); 
//	players.starting_position.reset();
//	if (position_widget.Get())
//		players.starting_position.reset(new Position(*position_widget.Get()));
	return true;
}
