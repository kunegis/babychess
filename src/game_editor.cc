#include "game_editor.hh"

#include "fen.hh"
#include "game_stream.hh"
#include "verbose.hh"
#include "nag.hh"
#include "pgn.hh"
#include "help_menu.hh"
#include "format_move_name.hh"

#include "../dep/strings.hh"

#include <ctype.h>
#include <gtk/gtkpaned.h>

#define s_comment_changed "comment_changed"
#define s_game_info_edited "game_info_edited"
#define s_export_options_edited "export_options_edited"

String Truncate(String text, unsigned length)
{
	assert (length >= 4);

	if (*text <= length)
		return text;

	return text.Sub(0, length - 3) + "...";
}

bool Game_Editor::DE_Open(Pointer <Box> parent)
{
	notation_preferences.Open(*this, "notation");
	notation_settings_preferences.Open(*this, "notation_settings");

#define GAME_EDITOR_NOTATION_MENU\
 { "/View/"N_("_Choose Notation"),   NULL, \
   MENU_CALLBACK, str_edit_notation,"<StockItem>",GTK_STOCK_PREFERENCES},\
 { "/View/"N_("_Notation Preferences"),   NULL, \
   MENU_CALLBACK, str_edit_notation_settings,"<StockItem>",GTK_STOCK_PREFERENCES},\

//		 { "/View/t",  NULL, NULL, 0, "<Tearoff>", 0},
		 /* This could be added everywhere to have tearoff
		  * menues consistently.  */

	static GtkItemFactoryEntry menu_items[] = 
		{{ "/"N_("_File"),                            NULL,
		   NULL,         0,                    "<Branch>",    0},
		 { "/"N_("_View"),                            NULL, 
		   NULL,          0,                          "<Branch>",    0},
		 { "/View/"N_("_Legal Moves"),                "<ctrl>l", 
		   MENU_CALLBACK, str_show_legal_list,     "<CheckItem>", 0},
		 { "/View/"N_("_Evaluator Process"),          NULL, 
		   MENU_CALLBACK, str_show_evaluator,      "<CheckItem>", 0},
		 { "/View/"N_("_Comment"),                    NULL, 
		   MENU_CALLBACK, str_show_comment,        "<CheckItem>", 0},
		 { "/View/"N_("Hide the Side _Bar"),                   NULL,
		   MENU_CALLBACK, str_hide_side_bar,       "<CheckItem>", 0},
		 { "/View/sep1",                                      NULL, 
		   NULL,          0,                          "<Separator>", 0},
		 GAME_EDITOR_NOTATION_MENU
		 BOARD_MENU
		 { "/"N_("_Game"),                           NULL, 
		   NULL,          0,                          "<Branch>",    0},
		 { "/Game/"N_("New Starting _Position"),            NULL,
		   MENU_CALLBACK, str_edit_position, NULL,          0},
		 { "/Game/sep0",                                      NULL,         
		   NULL,         0,                    "<Separator>", 0},
		 { "/Game/"N_("Go to the _Start"), "<alt>Home", 
		   MENU_CALLBACK, str_start,           "<StockItem>", GTK_STOCK_GOTO_FIRST},
		 { "/Game/"N_("Go _Back One Move"),        "<alt>Left",    
		   MENU_CALLBACK, str_back,            "<StockItem>", GTK_STOCK_GO_BACK},
		 { "/Game/"N_("Go _Forward One Move"),   "<alt>Right",  
		   MENU_CALLBACK, str_forward,      "<StockItem>", GTK_STOCK_GO_FORWARD},
		 { "/Game/"N_("Go to the _End"),   "<alt>End",    
		   MENU_CALLBACK, str_end,          "<StockItem>", GTK_STOCK_GOTO_LAST},
		 { "/Game/sep1",                       NULL,
		   NULL,          0,                          "<Separator>", 0},
		 { "/Game/"N_("Remove _End of Variation"),    NULL,
		   MENU_CALLBACK, str_remove_end,             NULL,          0},
		 { "/Game/"N_("Remove _Variation"),           NULL,
		   MENU_CALLBACK, str_variation_remove,    NULL,          0},
		 { "/Game/"N_("_Reparent Variation"),         NULL, 
		   MENU_CALLBACK, str_variation_make_parent,  NULL,          0},
		 { "/Game/"N_("Reparent Variation as _Main"), NULL,
		   MENU_CALLBACK, str_variation_make_main,    NULL,          0},
		 { "/Game/"N_("Leave _Variation"),            NULL, 
		   MENU_CALLBACK, str_variation_leave,        NULL,          0},
		 EVALUATOR_WIDGET_MENU
		 OPENING_WIDGET_MENU
		 HELP_MENU
		};

	if (! menubar.Open(parent(), sizeof(menu_items)/sizeof(menu_items[0]),
			    menu_items, *this))
		return false;
	Document_Editor <Game> ::Append_Menu(menubar); 

	/* toolbar */ 
	if (! toolbar.Open(parent())) return false; 
	Document_Editor <Game> ::Append_Tool_Buttons(toolbar);
	toolbar.Append_Space();
	start_button= toolbar.Append(GTK_STOCK_GOTO_FIRST, "start", 
				     _("Go to the start of the game"));
	back_button= toolbar.Append(GTK_STOCK_GO_BACK, "back", _("Go back one move"));
	forward_button= toolbar.Append(GTK_STOCK_GO_FORWARD, "forward",
				       _("Go forward one move"));
	end_button= toolbar.Append(GTK_STOCK_GOTO_LAST, "end", _("Go to the end of the game"));
	toolbar.Append_Space();
	evaluator_widget.Append_Tool_Buttons(toolbar); 

	toolbar.next_tooltip= _("Start / stop the engine");
	busy.Open(toolbar, "ev_busy"); 

	/* middle box */ 
	parent->Set_Next(true);
	if (! middle_paned.Open(parent()))
		return false;
	

	/* side box */ 
	if (! side_box.Open(middle_paned))     return false;

	/* opening */
	if (! opening_widget.Open(side_box, menubar, game, "opening"))
		return false;

	/* additional game info */
	if (! event_box.Open(side_box, dialog_unit))  return false;
	event_box.Set_Visible(false);
	if (! event_text.Open(event_box, "", 0.0, "style=\"italic\""))   return false;
	if (!  site_text.Open(event_box, "", 0.0, "style=\"italic\""))   return false;
	if (! round_text.Open(event_box, "", 0.0, "style=\"italic\""))   return false; 
	if (!  year_text.Open(event_box, "", 0.0, "style=\"italic\""))   return false; 

	/* game info */ 
	for (int cc= 0;  cc < 2;  ++cc)
	{
		if (! game_info_boxs[cc].Open(side_box))  return false;
		game_info_boxs[cc].Set_Visible(false); 
		if (! to_move_widgets[cc].Open(game_info_boxs[cc],
					       Chess_Info_Widget::t_empty))  
			return false;
		if (! chess_color_widgets[cc].Open(game_info_boxs[cc], cc)) return false;
		if (! player_names[cc].Open(game_info_boxs[cc])) return false;
	}

	/* Move list */ 
	side_box.Set_Next(true);
	if (! game_list.Open(side_box, 
			     game,
			     notation_preferences(), 
			     notation_settings_preferences(),
			     "move_list"))
		return false; 

	/* Comment */ 
	side_box.Set_Next(true);
	if (! comment_box.Open(side_box))//, Frame::Open_Info(GTK_SHADOW_IN, _("Comment"))))
		return false;
	if (! comment_label.Open(comment_box, _("Comment")))
		return false;
	comment_box.Set_Next(true);
	if (! comment.Open(comment_box, Text_View::Open_Info("", true, s_comment_changed)))
		return false;

	show_comment= true;
	if (! comment_diagram.Open(comment_box, false,
				   _("Diagram")))
		return false;
	if (! nag_menu.Open(comment_box, "nag")) return false;
	
	nag_menu << Format("(%s)", _("no annotation"));
	
	for (int i= 1;  i < nag_count;  ++i)
	{
		String nag= NAG(i), description= NAG_Description(i); 
		String text= Format("%s • %s", nag(), description());
		nag_menu << text;
	}
	nag_menu << Format("(%s)", _("another glpyh"));
	nag_menu.Update(); 

	comment_box.Set_Visible(false);

	/* Evaluator */ 
	if (! evaluator_widget.Open(side_box, 
				    "ew_play", 
				    "ew_changed",
				    game.Current_Position(),
				    game.Current_Generator(),
				    opening_widget,
				    notation_preferences(),
				    notation_settings_preferences(),
				    menubar,
				    busy))
		return false;
	show_evaluator_widget_on_start= true; 
	evaluator_widget.Set_Visible(false); 

	if (! promotion_chooser.Open(side_box))
		return false; 
	promotion_chooser.Set_Visible(false);

	/* Board */ 
	if (! board.Open(middle_paned, 
			 Board::Open_Info(game.Current_Position(), 
					  "dragged",
					  -1,
					  "msg_board_turn",
					  "msg_board_wheel_up",
					  "msg_board_wheel_down",
					  game.Current_Generator(),
					  opening_widget,
					  false,
					  menubar)))
		return false; 
	board.notation= &notation_preferences();
	board.notation_settings= &notation_settings_preferences(); 

	/* Statusbar */ 
	if (! statusbar.Open(parent(), 4))   return false; 
	board.statusbar= statusbar.Get(1);

	gtk_paned_set_position(GTK_PANED(middle_paned.Get()), dialog_unit*10*5/3);

	DO_Modified_Changed(Modified());

	Update_Position_Info();
	game_list.Update();
	Update_Location();
	Update_Game_Info();

	return true; 
}

void Game_Editor::MR_Receive(String message)
{
	if (!memcmp(message(), "board_", 6))
		board.MR_Receive(message);

	else if (!memcmp(message(), "opening_", 8))
		opening_widget.MR_Receive(message() + 8);

	else if (message == "remove_end")
	{
		game.Remove_End();
		game_list.Update_Length();
		Modify();
		Update_Location();
		Update_Legal_List(); 
		Update_Opening();
	}
	else if (message == "edit_position")
	{
		Position_Editor::next_turned= board.Turned(); 
		position_editor.Open("position_edited", 
				     *this,
				     game.Current_Position(),
				     this,
				     _("Set Up Position"));
	}
	else if (message == "back")
	{
		Go_Backward();
	}
	else if (message == "end")
	{
		Save_Comment();
		if (! game.Go_To_End())  return;
		Reset_Evaluator();
		Update_Position_Info();
		game_list.Update_Highlight();
		Update_Location();
		board.Invalidate_Position();
	}
	else if (message == "notation" ||
		 message == "notation_settings")
	{
		game_list.Update();
		Update_Legal_List(); 
		evaluator_widget.Set_Notation(notation_preferences());
		Update_Last_Move_Text(); 
	}
	else if (message == "position_edited")
	{
		Reset_Evaluator();
		Save_Comment();
		game.New(position_editor.Value());
		position_editor.Have_Read();
		promotion_chooser.Set_Visible(false);
		material_widget.Hide(); 
		Update_Position_Info();
		game_list.Update();
		Update_Location();
		board.Invalidate_Position();
		int new_first_move_number= game.Current_Position().previous_move_count / 2 + 1; 
		if (game.info.first_move_number != new_first_move_number)
			game.info.first_move_number= new_first_move_number; 
		Modify();
	}
	else if (message == "dragged")
	{
		Dragged();
	}
	else if (message == "forward")
	{
		Go_Forward();
	}
	else if (message == "start")
	{
		Save_Comment();
		int depth= game.Variation_Depth();
		int length= game.Location_Stack()[0];
		assert (depth >= 1);
		if (game.Go_To_Start())
		{
			Reset_Evaluator();
			Update_Position_Info();
			if (depth != 1)
			{
				game_list.Update_Length();
				game_list.Update_Content(length);
			}
			game_list.Update_Highlight();
			Update_Location();
			board.Invalidate_Position();
		}
	}
	else if (message == s_properties)
//	else if (message == "edit_game_info")
	{
		game_info_editor.Open(s_game_info_edited,
				      *this,
				      game.info,
				      this); 
	}
	else if (message == s_game_info_edited)
	{
		game.info= game_info_editor.Value();
		game_info_editor.Have_Read(); 
		Update_Game_Info();
		int new_previous_move_count=
			(game.info.first_move_number-1) * 2 
			+ game.First_Move(); 
		if (!game.Starting_Position().Is() && new_previous_move_count ||
		    new_previous_move_count != game.Current_Position().previous_move_count)
		{
			game.Set_Starting_Previous_Move_Count(new_previous_move_count); 
			game_list.Update_Length();
		}
		Update_Last_Move(); 
		Modify();
	}
	else if (message == s_export)
	{
		export_options_editor.Open
			(s_export_options_edited,
			 *this,
			 export_options,
			 this); 
		
	}
	else if (message == s_export_options_edited)
	{
		export_options= export_options_editor.Value();
		export_options_editor.Have_Read(); 
	}
	else if (message == "play_best")
	{
		if (! evaluator_widget.Started())
			return;
		int best_move= evaluator_widget.Get_Best_Move();
		if (best_move < 0)
			return;
		Play(best_move);
		return;
	}
	else if (message == "ew_play")
	{
		if (*game.Current_Generator() == 0)
			return;
		int best_move= evaluator_widget.Get_Best_Move();
		if (best_move != -1)
		{
			Play(best_move);
			gdk_beep();
		}
		return;
	}
	else if (!memcmp(message(), "ev_", 3))
		evaluator_widget.MR_Receive(message);
	else if (message == "ew_changed")
	{
		if (show_evaluator_widget_on_start)
		{
			evaluator_widget.Set_Visible(true);
			menubar.Set_Active("/View/Evaluator Process", true);
		}
	}
	else if (message == "hide_side_bar")
	{
		side_box.Set_Visible(! menubar.Active("/View/Hide the Side Bar")); 
	}
//		side_box.Set_Visible(! GTK_WIDGET_VISIBLE(side_box.Get()));
	else if (message == "show_evaluator")
	{
		evaluator_widget.Set_Visible(! GTK_WIDGET_VISIBLE(evaluator_widget.Get())); 
		show_evaluator_widget_on_start= false; 
		if (GTK_WIDGET_VISIBLE(evaluator_widget.Get()))
			side_box.Set_Visible(true); 
	}
	else if (message == "show_legal_list")
	{
		Update_Legal_List();
	}
	else if (message == "ev_run")
	{
		if (*game.Current_Generator() != 0)
		{
			if (show_evaluator_widget_on_start)
			{
				evaluator_widget.Set_Visible(true); 
				menubar.Set_Active("/View/Evaluator Process", true);
			}
			evaluator_widget.Start_Or_Play();
		}
	}
	else if (message == "move_list")
	{
		int location= game_list.Location(); 
		Save_Comment();
		if (game.Go_To_Overall_Location(location))
		{
			game_list.Update();
		}
		else
		{
			game_list.Update_Highlight();
		}
		Update_Location();
		Reset_Evaluator();
		Update_Position_Info();
		board.Invalidate_Position();
	}
	else if (message == "msg_board_turn")
	{
		material_widget.Set_Turned(game.Current_Position(), board.Turned());
	}
	else if (message == "msg_board_wheel_up")
		Go_Backward();
	else if (message == "msg_board_wheel_down")
		Go_Forward(); 
	else if (message == "show_comment")
	{
		if (GTK_WIDGET_VISIBLE(comment_box.Get()))
		{
			Save_Comment();
			comment_box.Set_Visible(false);
			show_comment= false;
		}
		else
		{
			Update_Comment();
			comment_box.Set_Visible(true);
		}
	}
	else if (message == "legal")
	{
		int index= legal_list->Selection();
		if (index < 0)  return;
		Verbose_Int(index);
		move_t move= legal_moves[index];
		Verbose("%d-%d", move & 0xFF, (move >> 8) & 0xFF);
		Play(game.Current_Generator().Square_Move
		     (move & 0xFF ,
		      (move >> 8) & 0xFF)
		     + Move::Promotion_Piece_Index((move >> 24)&0xF));
	}
	else if (message == "variation_remove")
	{
		game.Remove_Variation();
		Update_Position_Info();
		game_list.Update_Length();
		game_list.Update_Content(game.Overall_Location());
		game_list.Update_Highlight();
		Update_Location();
		board.Invalidate_Position(); 
	}
	else if (message == "variation_leave")
	{
		if (! game.Leave_Variation()) return;
		Reset_Evaluator(); 
		game_list.Update_Length();
		game_list.Update_Content(game.Overall_Location());
		game_list.Update_Highlight();
		Update_Location();
		Update_Position_Info(); 
		board.Invalidate_Position(); 
	}
	else if (message == "variation_make_parent")
	{
		if (game.Make_Parent())
		{
			game_list.Update_Content();
			Update_Variation_Depth();
			Modify();
		}
	}
	else if (message == "variation_make_main")
	{
		if (game.Make_Main())
		{
			game_list.Update_Content();
			Update_Variation_Depth(); 
			Modify(); 
		}
	}		
	else if (message == "edit_notation")
	{
		notation_preferences.Edit(); 
	}
	else if (message == "edit_notation_settings")
	{
		notation_settings_preferences.Edit(); 
	}
	else if (message == "nag")
	{
		int nag= nag_menu.Selection();
		int location= game.Overall_Location();
		if (location == 0)
		{
			assert(0);
			return;
		}
		game.Set_NAG(nag);
		Update_Last_Move(); 
		game_list.Update_Content_One(location-1);
	}
	else if (message == "opening")
	{
		Update_Legal_List(); 
	}	       
	else if (message == s_comment_changed)
	{
		Modify(); 
	}

	else 
		Document_Editor <Game> ::MR_Receive(message);
}

void Game_Editor::Wdg_Close()
{
	Reset_Evaluator();
	position_editor.Close();
	notation_preferences.Close();
	notation_settings_preferences.Close();
	game_info_editor.Close(); 
	export_options_editor.Close(); 
	Save_Comment();
	Document_Editor <Game> ::Wdg_Close();
}

void Game_Editor::EW_Set_Value(Pointer <const Game> value)
{
	/* If the game was opened through options, ignore VALUE. 
	 */
	if (game.Is())  return;
	
	if (!value)
		game.New();
	else
		game= value();
}

void Game_Editor::Update_To_Move()
{
	/* status bar */ 
  	statusbar.Set(1, game.To_Move_Description());

	/* game info widget */

	to_move_widgets[cc_w].Set(game.Current_Position().To_Move() == cc_w 
				  ? Chess_Info_Widget::t_down : Chess_Info_Widget::t_empty);
	to_move_widgets[cc_b].Set(game.Current_Position().To_Move() == cc_b 
				  ? Chess_Info_Widget::t_up : Chess_Info_Widget::t_empty);
}

void Game_Editor::Update_Last_Move()
{
	Update_Last_Move_Text();

	int location= game.Overall_Location();
	if (location != 0)
	{
		gtk_widget_set_sensitive(nag_menu.Get(), true);
		int nag= game.Overall_NAG(location - 1);
		assert (nag >= 0);
		int s= std::min(nag, (int) nag_count);
		nag_menu.Set_Selection(s);
	}
	else
	{
		nag_menu.Set_Selection(0); 
		gtk_widget_set_sensitive(nag_menu.Get(), false);
	}
}

const Game &Game_Editor::EW_Get_Value() 
{
	if (Is())  Save_Comment();
	return game;
}

void Game_Editor::Play(int move_index, bool update, bool as_main)
{
	Move move= game.Current_Generator()[move_index]; 

	/* move is next in current variation:  go forward */ 

	if (game.Current_Length() > game.Current_Location()     &&
	    (game.Current_Move(game.Current_Location()).Same_As
	     (game.Current_Generator()[move_index])))
	{
		Go_Forward(update);
		return;
	}

	/* Move is a variation */ 

	if (game.Current_Variation().Have_Variations(game.Current_Location()))
	{
		for (int i= 0;  
		     i < *game.Current_Variation().Get_Variations
			     (game.Current_Location());
		     ++i)
		{
			const Variation &variation= game.Current_Variation()
				.Get_Variations(game.Current_Location())[i]();

			if (variation.Length() != 0 &&
			    move.Same_As(variation[0]))
			{
				Reset_Evaluator(); 
				if (update)
					Save_Comment();
				game.Play_Variation(i);
				if (as_main)
					game.Make_Parent(); 
				if (update)
				{
					game_list.Update();
					game_list.Update_Highlight();
					Update_Location();
					Update_Position_Info();
				}
				board.Invalidate_Move_Squares(move); 
				return;
			}
		}
	}

	/* Play the move */

	Reset_Evaluator();

	Move_Name move_name= game.Current_Generator().Get_Move_Name(move_index); 
	
	if (update)   Save_Comment();
	if (game.Current_Location() != game.Current_Variation().Length())
		game.Append_Variation();

	game.Append_Move(move_index);
	Update_Opening();

	if (as_main)
		game.Make_Parent();

	/* If the move ends the main variation with mate or stalemate,
	 * change game info.    */
	if (move_name.force &&
	    game.Variation_Depth() == 1)
	{
		int result;
		
		if (move_name.force)
		{
			if (move_name.check)
				result= (game.Current_Position().To_Move() == cc_w
					 ? re_black
					 : re_white);
			else 
				result= re_draw; 
		}

		if (game_info_editor.Is_Editing())
		{
			game_info_editor().Set_Result(result); 
		}
		else
		{
			game.info.result= result; 
		}
	}

	/* update */ 

	board.Invalidate_Move_Squares(move);
	if (update)
	{
		game_list.Update_Length();
		game_list.Update_Content(game.Overall_Location()-1);

		game_list.Update_Highlight();
		Update_Location();
		Update_Position_Info();
	}
	Modify();
}

bool Game_Editor::EW_Change_Value(const Game &new_game)
{
	game= new_game;
	Reset_Evaluator();
	game_list.Update();
	Update_Location(); 
	promotion_chooser.Set_Visible(false);
	Update_Position_Info();
	board.Invalidate_Position();
	Update_Game_Info(); 
	return true;
}

bool Game_Editor::EW_Try_Load(String filename, 
			      Pointer <Game> game)
{
	Position position;
	if (Read_From_File(filename(), position))
	{
		game().New(position);
		return true;
	}
	else
		return false;
}

void Game_Editor::Update_Legal_List()
{
	bool show_legal= menubar.Active("/View/Legal Moves");
	
	/**** info */
	if (! show_legal)
	{
		if (legal_info.Is())
			legal_info.Set_Visible(false);
	}
	else
	{
		if (legal_info.Is())
			legal_info.Set_Visible();
		else
			legal_info.Open(side_box);
		if (legal_info.Is())
			legal_info.Set(game.Current_Generator(),
				       notation_preferences()); 
	}

	/**** legal moves */ 
	int current_location= game.Current_Location();

	/* whether there is more than one variation in the current
	 * position */ 
	bool have_variations=  current_location != *game.Current_Variation()
		&& game.Current_Variation().Have_Variations(current_location)
		&& *game.Current_Variation().Get_Variations(current_location);
	bool show_variations= have_variations || 
		(legal_list.Is() && GTK_WIDGET_VISIBLE(legal_list.Get()));

	if (! (show_legal || show_variations))  
		/* The legal move list is not shown at all. */
	{
		if (legal_list.Is()) legal_list.Set_Visible(false);
		return;
	}

	/* create it if still not opened */ 
	if (! legal_list.Is())
	{
		if (! legal_info.Is())
		{
			legal_info.Open(side_box);
			legal_info.Set_Visible(false);
		}
		side_box.Set_Next(true);
		List::Open_Info oi(2, scr_vertical, "legal", NULL);
		if (! legal_list.Open (side_box, oi))  return; 
	}

	/** add subvariations of the game */
	legal_list->Set_Row_Count(0);

	/* For now there cannot be subvariations at the end of a
	 * variation.  This will change in the next version.  (with
	 * the rewrite of the GAME module)  */
	if (current_location != *game.Current_Variation())
	{
		/* main variation */ 
		legal_list->Set_Row_Count(1);
		legal_list->Set
			(0, 0,
			 game.Overall_Move_Text
			 (game.Overall_Location(), 
			  notation_preferences(),
			  notation_settings_preferences()));
		
		legal_moves= 1;
		legal_moves[0]= game.Overall_Move
			(game.Overall_Location()).Small();

		/* subvariations */
		if (game.Current_Variation().Have_Variations(current_location))
		{
			int variation_count= *game.Current_Variation()
				.Get_Variations(current_location);
			legal_list->Set_Row_Count(1 + variation_count);
			legal_moves= 1 + variation_count;
			for (int i= 0;  i < variation_count;  ++i)
			{
				legal_list->Set
					(0, 1 + i,
					 game.Format_Variation_Move
					 (i,
					  notation_preferences(), 
					  notation_settings_preferences()));
				legal_moves[1+i]=
					game.Variation_Move(i).Small();
				Verbose("%d-%d", 
				    game.Variation_Move(i).from,
				    game.Variation_Move(i).to);
			}
		}
	}
	else  
		legal_moves= 0;
	
        /** add other legal moves */
	if (show_legal)
	{
		int pre= *legal_moves;
		legal_moves= *game.Current_Generator();
		legal_list->Set_Row_Count(*game.Current_Generator()); 
		int next= pre;
		for (int i= 0;  i < *game.Current_Generator();  ++i)
		{
			move_t move= game.Current_Generator()[i].Small();
			bool found= false;
			for (int j= 0;  j < pre;  ++j)
			{
				if (legal_moves[j] == move)
				{
					found= true;
					j= pre;
				}
			}
			if (! found)
			{
				legal_moves[next]= move;
				legal_list->Set
					(0, next++,
					 Format_Move_Name
					 (game.Current_Generator().Get_Move_Name(i),
					  notation_preferences(),
					  notation_settings_preferences())); 
			}
		}
		assert (next == *game.Current_Generator());
	}

	/* Opening names */ 
	for (int i= 0;  i < *legal_moves;  ++i)
	{
		move_t move= legal_moves[i];
		legal_list->Set(1, i, opening_widget.Name(move));
	}

	legal_list.Set_Visible(); 
}

void Game_Editor::Update_Position_Info()
{
	Update_Board_Selection();
	Update_Last_Move();
	Update_To_Move();
	Update_Opening();
	Update_Legal_List(); 
	Update_Comment();

	/* Material */ 
	material_widget.Update(game.Current_Position(),
			       board.Turned(),
			       side_box);

	/* Evaluator */
	if (evaluator_widget.settings.mode == em_analyze)
		evaluator_widget.Start_Or_Play();

	/* Variation depth */ 
	Update_Variation_Depth();

	/* Promotion chooser visibility */ 
	promotion_chooser.Update(game.Current_Position());
}

void Game_Editor::DE_Reset()
{
	Reset_Evaluator();

	game.New();

	if (legal_list.Is())  legal_list.Set_Visible(false);
	material_widget.Hide();
	promotion_chooser.Set_Visible(false);
	Update_Position_Info();
	game_list.Update();
	Update_Location(); 
	board.Invalidate_Position();
	Update_Game_Info(); 
}

void Game_Editor::Go_Backward()
{
	Save_Comment();

	if (game.Overall_Location() == 0)
		return; 

	Move move= game.Overall_Move(game.Overall_Location() - 1);

	int depth= game.Variation_Depth();
	if (! game.Go_Backward())
		return;
	/* sync */ 

	Reset_Evaluator();
	Update_Position_Info();
	board.Invalidate_Move_Squares(move); /* squares of move taken back */
	if (depth != game.Variation_Depth())
	{
		game_list.Update_Length();
		game_list.Update_Content(game.Overall_Location());
		game_list.Update_Highlight();
	}
	else
		game_list.Update_Highlight();
	Update_Location();
}


void Game_Editor::Save_Comment()
{
	Verbose_Function();

	if (! GTK_WIDGET_VISIBLE(comment_box.Get()))
		return;

	String text= comment().Get_Text();
	if (comment_diagram.Checked())
		text= String('\04') + text;
	Verbose_Str(text());
	game.Set_Comment(text);
}

void Game_Editor::Update_Comment()
{
	String text= game.Comment();

	/* Empty comment */ 
	if (! text.Is())
	{
		if (GTK_WIDGET_VISIBLE(comment_box.Get()))
		{
			comment().Set_Text("");
			comment_diagram.Set_Checked(false);
		}
		return;
	}

	/* make visible */ 
	if (! GTK_WIDGET_VISIBLE(comment_box.Get()))
	{
		if (show_comment)
		{
			comment_box.Set_Visible(true);
			menubar.Set_Active("/View/Comment", true);
		}
		else
			return;
	}

	/* find whether the comment begins with [[:space:]]*^D  */
	int i= 0;
	while (isspace(text[i]))  ++i;
	if (text[i] != '\04')  i= -1;  /* Control-D */ 
	comment_diagram.Set_Checked(i >= 0);
	
	comment().Clear();
	comment().Append_Text(text() + (i < 0 ? 0 : i+1)); 
}

void Game_Editor::Go_Forward(bool update)
{
	if (update)
		Save_Comment();
	if (! game.Go_Forward())
		return;

	/* sync */ 

	Reset_Evaluator();
	if (update)
	{
		Update_Position_Info();
		game_list.Update_Highlight();
		Update_Location(); 
	}

	board.Invalidate_Move_Squares(game.Overall_Move(game.Overall_Location()
							    - 1)); 
}

void Game_Editor::Update_Game_Info()
{
	/* Player names */ 
	String white= game.info.players[cc_w];
	String black= game.info.players[cc_b];

	if (! (white || black))
	{
		game_info_boxs[0].Set_Visible(false);
		game_info_boxs[1].Set_Visible(false);
	}
	else
	{
		game_info_boxs[0].Set_Visible(true); 
		game_info_boxs[1].Set_Visible(true); 
		if (white.Is())
			white= Truncate(white, 20);
		else
			white= _("White");
		if (black.Is())
			black= Truncate(black, 20);
		else
			black= _("Black");
		player_names[0].Set(white);
		player_names[1].Set(black); 
	}
	
	/* Additional info */ 
	String event= Truncate(game.info.event, 20);
	String site= Truncate(game.info.site, 20);
	String round= Truncate(game.info.round, 7);
	if (event || site || round)
	{
		event_box.Set_Visible();
		event_text.Set(event + (event || site ? "," : ""));
		site_text.Set(site);
		if (round)
		{
			round_text.Set(Format("[%s]", round()));
		}
		else
			round_text.Set("");
	}
	else
	{
		event_box.Set_Visible(false);
	}
	int year= game.info.date_year;
	if (year < 0)  year_text.Set("");
	else           year_text.Set(Format("%d", year));
}

void Game_Editor::Reset_Evaluator()
{
	evaluator_widget.Reset();
}

void Game_Editor::Update_Board_Selection()
{
	if (game.Variation_Depth() == 1 && game.Current_Location() == 0)
		board.Clear_Last_Move();
	else
	{
		const Move &move= game.Overall_Move
			(game.Overall_Location() - 1);
		board.Set_Last_Move(move.from, move.to);
	}
}

void Game_Editor::DO_Modified_Changed(int modified)
{
	statusbar.Set(3, modified>=2 ? _("Modified") : "");
}

void Game_Editor::EW_Caption_Changed()
{
	position_editor.Update_Caption();
	game_info_editor.Update_Caption(); 
	export_options_editor.Update_Caption();
	notation_preferences.Caption_Changed(); 
	notation_settings_preferences.Caption_Changed();
	board.Caption_Changed();
}

void Game_Editor::Update_Location()
{
	bool nstart= game.Overall_Location() != 0;
	menubar.Set_Sensitive("/Game/Go to the Start", nstart);
	menubar.Set_Sensitive("/Game/Go Back One Move", nstart);
	gtk_widget_set_sensitive(start_button, nstart);
	gtk_widget_set_sensitive(back_button, nstart);
	bool nend= game.Overall_Location() != game.Overall_Length();
	menubar.Set_Sensitive("/Game/Go to the End", nend);
	menubar.Set_Sensitive("/Game/Go Forward One Move", nend);
	menubar.Set_Sensitive("/Game/Remove End of Variation", nend);
	gtk_widget_set_sensitive(forward_button, nend);
	gtk_widget_set_sensitive(end_button, nend);
	bool var= game.Variation_Depth() > 1;
	menubar.Set_Sensitive("/Game/Remove Variation", var);
	menubar.Set_Sensitive("/Game/Reparent Variation", var);
	menubar.Set_Sensitive("/Game/Reparent Variation as Main", var);
	menubar.Set_Sensitive("/Game/Leave Variation", var);
}

bool Game_Editor::DE_Set_Args(Open_Info &open_info, 
			      bool &anonymous)
{
	bool correct= true;
	int c;
	while ((c = getopt(open_info.argc, open_info.argv, "p:a")) >= 0)
	{
		switch (c)
		{
		case 'p':
			if (optarg == NULL || ! game.New(optarg))
				correct= false;
			break;

		case 'a':
			anonymous= true;
			break;
		}
	}
	memmove(open_info.argv+1, open_info.argv+optind,
		sizeof(char *) * (open_info.argc - optind));
	open_info.argc -= (optind - 1);
	
	return correct; 
}

void Game_Editor::Dragged()
{
	int from, to;
	guint state; 
	if (! board.Get_Squares(from, to, &state)) return; 
	state &= GDK_CONTROL_MASK; /* non-zero when CONTROL is pressed */ 

	/* piece was dragged out of the board */ 
	if (to == 0)  return; 

	/* play a legal move */ 
	int index= game.Current_Generator().Square_Move(from, to);
	if (index < 0)
		index= game.Current_Generator().Square_Move(to, from);
	if (index >= 0) 
	{
		if (game.Current_Generator()[index].promotion & pi_color)
			/* move is a promotion */
		{
			index += promotion_chooser.Selection();
		}

		Play(index, true, state);

		if (evaluator_widget.settings.mode != em_off)
			evaluator_widget.Start_Or_Play();
		return;
	}
	
	/* no such legal move */

	/* If it was the last move in the game, take it back.
	 * Castling can be taken back by taking back the rook move.
	 */  

	if (game.Overall_Location() == 0)   return; 

	const Move &last_move= game.Overall_Move(game.Overall_Location() - 1); 
	if ((last_move.from == to   &&  last_move.to == from)                         ||
	    (last_move.castling &&
	     ((last_move.castling == wO_O   && from == sq_f1 && to == sq_h1)  ||
	      (last_move.castling == wO_O_O && from == sq_d1 && to == sq_a1)  ||
	      (last_move.castling == bO_O   && from == sq_f8 && to == sq_h8)  ||
	      (last_move.castling == bO_O_O && from == sq_d8 && to == sq_a8))))
	{
		Go_Backward();
		return; 
	}

	/* take back last move and play another move with the same
	 * piece */ 
	if (last_move.to != from)    return;  

	/* determine index of last move.  -1 if a main variation or
	 * the index in the variation list.  
	 */ 
	Save_Comment();
	int vindex= -1;
	if (! game.Go_Backward())  
	{
		return; 
	}
	
	if (game.Current_Variation().Have_Variations(game.Current_Location()))
		for (int i= 0;  
		     i < *game.Current_Variation().Get_Variations
			     (game.Current_Location());  
		     ++i)
			if (last_move.Same_As
			    (game.Current_Variation().Get_Variations
			     (game.Current_Location())[i]()[0]))
			{
				vindex= i;
				break;
			}

	for (int i= 0;  i < *game.Current_Generator();  ++i)
	{
		Move new_move= game.Current_Generator()[i];
		if (new_move.from == last_move.from  &&
		    new_move.to != last_move.to      &&
		    new_move.to == to)
		{
			Play(i, false);
			if (state)
				game.Make_Parent(); 
			board.Invalidate_Move_Squares(last_move);
			board.Invalidate_Move_Squares(new_move);
			game_list.Update_Length();
			game_list.Update_Content(game.Overall_Location()-1);
			Update_Location();
			Update_Position_Info();
			Modify();
			if (evaluator_widget.settings.mode != em_off)
//			if (evaluator_widget.settings.start == st_auto)
			{
				evaluator_widget.Start_Or_Play();
			}
			return; 
		}
	}

	/* no such other move */ 
	/* replay the move that was taken back */ 
	if (vindex < 0)   game.Go_Forward();
	else              game.Play_Variation(vindex); 
}

void Game_Editor::Update_Variation_Depth()
{
	int depth= game.Variation_Depth()-1;
	assert (depth >= 0);
	statusbar.Set(2, depth 
		       ? Format(_("Subvariation level %d"), depth)
		       : _("Main variation"));
}

void Game_Editor::Update_Opening()
{
	opening_widget.Update(); 
}

void Game_Editor::Update_Last_Move_Text()
{
	String text= game.Format_Last_Move
		(notation_preferences(),
		 notation_settings_preferences()); 
	statusbar.Set(0, text);
}
