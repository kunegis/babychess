#ifndef PLAY_WINDOW_HH
#define PLAY_WINDOW_HH

#include "box.hh"
#include "board.hh"
#include "game.hh"
#include "process_open_info.hh"
#include "play_player.hh"
#include "play_info_editor.hh"
#include "statusbar.hh"
#include "promotion_chooser.hh"
#include "players_display.hh"
#include "material_widget.hh"
#include "notation_editor.hh"
#include "ask_dialog.hh"
#include "opening_widget.hh"
#include "game_list.hh"
#include "file_dialog.hh"
#include "window.hh"
#include "notation_settings_editor.hh"

#include <memory>

class Play_Window
	:  public Window
{
public:
	typedef Process_Open_Info Open_Info; 

	bool Open(Open_Info);

private:
	V_Box overall_box; 
	H_Box main_box;
	V_Box side_box; 
	Players_Display players_display; 
	Opening_Widget opening_widget; 
	Menubar menubar; 
	Toolbar toolbar; 

	Material_Widget material_widget; 
	Promotion_Chooser promotion_chooser; 
	Board board;
	Statusbar statusbar; 
	/* 0: Last move+[finished]
	 * 1: To move, board
	 */
	Game_List game_list; 

	Play_Info play_info;
	std::auto_ptr <Play_Player> play_players[2];
	/* Empty when played by human  */
	Owned_Editor <Play_Info_Editor> play_info_editor; 
	Preferences <Notation_Editor> notation_preferences;
	Preferences <Notation_Settings_Editor> notation_settings_preferences; 
	bool playing;
	bool modified; 
	Ask_Dialog ask_dialog; 
	Game game;
	/* The game may be non-normalized when a move was taken back.
	 * Move are always appended to the current variation, which is
	 * empty after taking back a move.  Therefore, Go_Backward()
	 * must do special things:  leave the current variation if it is
	 * empty before taking back the last move.  When Book will be
	 * used instead of Game, all this will go away. 
	 */
	File_Dialog file_dialog; 

	void Wdg_Close();
	bool Wdg_Close_Query();
	
	void MR_Receive(String);

	void Update_Last_Move();
	void Update_Position();
	void Check_End_Of_Game(); 
	void Finish_Game();
	void Update_Playing(); 
	bool Go_Backward();
	void New();
};

#endif /* ! PLAY_WINDOW_HH */
