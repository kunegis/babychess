#ifndef GAME_EDITOR_HH
#define GAME_EDITOR_HH

//#include "game.hh"
#include "game_stream.hh"
#include "board.hh"
#include "toolbar.hh"
#include "statusbar.hh"
#include "position_editor.hh"
#include "evaluator_widget.hh"
#include "document_editor.hh"
#include "material_widget.hh"
#include "game_info_editor.hh"
#include "legal_info_widget.hh"
#include "preferences.hh"
#include "promotion_chooser.hh"
#include "paned.hh"
#include "text_view.hh"
#include "chess_info_widget.hh"
#include "option_menu.hh"
#include "opening_widget.hh"
#include "game_list.hh"
#include "notation_settings_editor.hh"
#include "scrolled.hh"
#include "export_options_editor.hh"

class Game_Editor
	:  public Document_Editor <Game> 
{
public:
	const Game &EW_Get_Value();

private:

	Game game;
	bool show_evaluator_widget_on_start;
	bool show_comment;
	Preferences <Notation_Editor> notation_preferences;
	Preferences <Notation_Settings_Editor> notation_settings_preferences;

	Owned_Editor <Position_Editor> position_editor;
	Owned_Editor <Game_Info_Editor> game_info_editor;
	Owned_Editor <Export_Options_Editor> export_options_editor; 

	Vector <move_t> legal_moves;
	/* The index is the same index as in the legal move list.  The
	 * value is the number of that move in the generator. 
	 */
	Export_Options export_options; 

	Toolbar toolbar;
	Promotion_Chooser promotion_chooser;
	H_Paned middle_paned;
	V_Box side_box;
	Evaluator_Widget evaluator_widget;
	Material_Widget material_widget;
	Board board;
	Statusbar statusbar;
	/* 0: Last move 
	 * 1: To move, board
	 * 2: Subvariation level
	 * 3: Modified
	 */
	H_Box game_info_boxs[2];
	Chess_Info_Widget to_move_widgets[2];
	Chess_Info_Widget chess_color_widgets[2];
	Label player_names[2];
	Game_List game_list; 
	Legal_Info_Widget legal_info;
	List legal_list;
	V_Box comment_box;
	Title_Label comment_label;
	Scrolled <Text_View> comment;
	Check_Box comment_diagram; 
	Option_Menu nag_menu;
	H_Box event_box;
	Label event_text, site_text, round_text, year_text; 
	Busy busy; 
	Opening_Widget opening_widget; 
	GtkWidget *start_button, *back_button, *forward_button, *end_button;

	void Wdg_Close();

	void MR_Receive(String);


	const char *EW_Type_Name() const 
		{ return _("Chess Game");   }

	void EW_Set_Value(Pointer <const Game>);
	int EW_Use()
		{
			return use_ok;
		}
	bool EW_Change_Value(const Game &);
	bool EW_Try_Load(String, Pointer <Game>);
	void EW_Caption_Changed();

	void DE_Reset();
	const char *DE_Mode() const
		{ return "--edit-game";   }
	bool DE_Set_Args(Open_Info &, bool &);
	const char *DE_Text_New() const { return N_("New game"); }
	const char *DE_Text_Open() const{ return N_("Open a game");}
	const char *DE_Text_Save() const{ return N_("Save this game");}
	bool DE_Show_Properties() const
		{  return true; }
	bool DE_Show_Export() const
		{  return true; }
	bool DE_Open(Pointer <Box>);
	
	void DO_Modified_Changed(int);

	void Update_To_Move();
	void Update_Last_Move();
	void Update_Last_Move_Text(); 

	void Update_Legal_List(); 
	void Play(int move_index, 
		  bool update_save= true,
		  bool as_main= false);
	/* Play the move and update everything.  Start the engine if
	 * settings.reply==re_auto. 
	 * Only do updates and comment saving if UPDATE. 
	 */
	void Update_Position_Info();
	/* Do everything when the position has changed, except
	 * updating the board, the move list and the variation list.  
	 */
	void Go_Forward(bool update= true);
	void Go_Backward();
	void Save_Comment();
 	void Update_Comment();
	void Update_Game_Info();
	bool Open_Variation_List();
	void Reset_Evaluator();
	void Update_Board_Selection();
	void Update_Location();
	/* Update sensitiveness of menu entries and tool buttons such as
	 * "forward".   */
	void Update_Variation_Depth();
	/* In the status bar.  */
	void Update_Opening();
	void Dragged(); 
};

#endif /* ! GAME_EDITOR_HH */
