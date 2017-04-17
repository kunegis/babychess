#ifndef INTERNET_WINDOW_HH
#define INTERNET_WINDOW_HH

#include "statusbar.hh"
#include "internet.hh"
#include "board.hh"
#include "internet_account_editor.hh"
#include "input_reader.hh"
#include "internet_interaction_window.hh"
#include "legal_move_list.hh"
#include "notation_editor.hh"
#include "preferences.hh"
//#include "notation_stream.hh"
#include "material_widget.hh"
#include "promotion_chooser.hh"
//#include "menubar.hh"
#include "process_open_info.hh"
#include "gettext.hh"
#include "toolbar.hh"
#include "players_display.hh"
#include "window.hh"
#include "notation_settings_editor.hh"

class Internet_Window
	:  public Window,
	   private Internet,
	   public Input_Reader
{
public:
	typedef Process_Open_Info Open_Info; 

	bool Open(Open_Info);

private:
	Board board;
	Statusbar statusbar;
	Internet_Account internet_account;
	Owned_Editor <Internet_Account_Editor> account_editor;
	Internet_Interaction_Window interaction_window;
	Legal_Move_List legal_move_list; 
	/* Always visible if open */
	Material_Widget material_widget;
	Promotion_Chooser promotion_chooser; 
	GtkWidget *login_button, *logout_button; 

	Preferences <Notation_Editor> notation_preferences;
	Preferences <Notation_Settings_Editor> notation_settings_preferences; 

	V_Box overall_box; 
	Menubar menubar;
	Toolbar toolbar; 
	H_Box top_box;
	Players_Display players_display;
	V_Box side_box;

	void Wdg_Close();
	
	void MR_Receive(String);

	void Internet_Chess_Changed();
	void Internet_Chess_New_Game();
	void Internet_Chess_Finished();
	void Internet_Chess_Playing(String);
	void Internet_Chess_Message(String _message);
	void Internet_Chess_Clock(int, int);

	void IR_Input();

	void Moved();
	void Seek(int, int);
	void Login();
	void Logout(bool omit_updates= false);
	void Update_Position();
	void Open_Interaction_Window();
	void User_Play(int move_index,
		       int from= -1,
		       int to= -1);
	void Update_Legal_Moves();
	void Update_Connected(String host= "", String user= "");
	/* If HOST is empty, display as not connected, else display as
	 * connected to that host. 
	 */
};

#endif /* ! INTERNET_WINDOW_HH */
