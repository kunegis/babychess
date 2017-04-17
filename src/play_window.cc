#include "play_window.hh"

#include "help_menu.hh"
#include "parse_move.hh"
#include "inform.hh"
//#include "ask.hh"
#include "game_stream.hh"
#include "gui_execute.hh"

#include "../dep/strings.hh"

#define s_play_info_edited "play_info_edited"

bool Play_Window::Open(Open_Info)
{
	notation_preferences.Open((*this), "notation");
	notation_settings_preferences.Open(*this, "notation_settings");
	if (! Window::Open
	    (_("Play Chess"), 
	     (GdkPoint){25*dialog_unit*5/3, 17*dialog_unit*5/3}))  
		return false;
	if (! overall_box.Open(this))  goto close; 


	static GtkItemFactoryEntry menu_items[] = 
		{{"/"N_("_Game"), NULL, NULL, 0, "<Branch>", 0},
		 {"/Game/"N_("_New"), NULL,
		  MENU_CALLBACK, str_new, "<StockItem>", GTK_STOCK_NEW},
		 {"/Game/"N_("_Save"), NULL, 
		  MENU_CALLBACK, str_save, "<StockItem>", GTK_STOCK_SAVE},
		 { "/Game/sep1",            NULL,         
		   NULL,          0,                   "<Separator>", 0},
		 { "/Game/"N_("_Pause"), NULL, MENU_CALLBACK,
		   str_pause, "<StockItem>", GTK_STOCK_MEDIA_PAUSE},
		 { "/Game/"N_("Take _Back Last Move"), NULL, MENU_CALLBACK,
		   str_take_back, "<StockItem>", GTK_STOCK_UNDO},
		 { "/Game/"N_("_Resign"), NULL, MENU_CALLBACK, str_resign,
		   NULL, 0},
		 { "/Game/"N_("Open Game in Game _Editor"),NULL,MENU_CALLBACK,
		   str_open_in_editor, NULL, 0},
		 { "/Game/sep2",            NULL,         
		   NULL,          0,                   "<Separator>", 0},
		 { "/Game/"N_("_Close"),       NULL, 
		   MENU_CALLBACK, str_ok,    "<StockItem>", GTK_STOCK_CLOSE},

		 { "/"N_("_View"),            NULL,         
		   NULL,          0,                   "<Branch>",    0},
		 { "/View/"N_("_Choose Notation"),NULL,
		   MENU_CALLBACK, str_edit_notation, "<StockItem>",  GTK_STOCK_PREFERENCES},
		 { "/View/"N_("_Notation Preferences"),   NULL, 
		   MENU_CALLBACK, str_edit_notation_settings,"<StockItem>",GTK_STOCK_PREFERENCES},
		 BOARD_MENU
		 OPENING_WIDGET_MENU
		 HELP_MENU
		};

	if (! menubar.Open(overall_box, 
			   sizeof (menu_items) / sizeof (GtkItemFactoryEntry),
			   menu_items,
			   *this))  goto close;
	game.New();
	playing= false;
	modified= false;

	if (! toolbar.Open(overall_box))  return false;
	toolbar.Append(GTK_STOCK_NEW, s_new, 
		       _("New game"));
	toolbar.Append(GTK_STOCK_SAVE, s_save,
		       _("Save the game"));
	toolbar.Append_Space();
	toolbar.Append(GTK_STOCK_MEDIA_PAUSE, s_pause,
		       _("Pause the game"));
	toolbar.Append(GTK_STOCK_UNDO, s_take_back,
		       _("Take back last move"));
	

	overall_box.Set_Next(true); 
	if (! main_box.Open(overall_box))  goto close;
	if (! side_box.Open(main_box))  goto close;
	if (! players_display.Open(side_box))  goto close;
	if (! opening_widget.Open(side_box, menubar, game, "opening"))
		goto close;
	side_box.Set_Next(true); 
	if (! game_list.Open(side_box, game, notation_preferences(),
			     notation_settings_preferences(),
			     "game_list"))
		goto close; 
//	if (! material_widget.Open(side_box))  goto close;
	if (! promotion_chooser.Open(side_box)) goto close; 
	promotion_chooser.Set_Visible(false);

	if (! board.Open(main_box,
			 Board::Open_Info
			 (game.Current_Position(),
			  "move",
			  "",
			  "turn", "", "",
			  game.Current_Generator(),
			  opening_widget,
			  false,
			  menubar)))

		goto close;

	board.notation= &notation_preferences();
	board.notation_settings= &notation_settings_preferences();
	
	if (! statusbar.Open(overall_box, 2))  goto close; 
	board.statusbar= statusbar.Get(1); 

	play_info.Reset();
	New();

	Update_Position();
	opening_widget.Update();
	Update_Playing();

	return true;

 close:
	Window::Close();
	return false;
}

void Play_Window::MR_Receive(String message)
{
	if (!memcmp(message(), "opening_", 8))
		opening_widget.MR_Receive(message() + 8);
	else if (!memcmp(message(), "board_", 6))
		board.MR_Receive(message);

	else if (message == s_play_info_edited) /* New game */ 
	{
		/* Save last game */ 
		if (modified)
		{
			ask_dialog.Open
				(_("Save the game before beginning a new game?"),
				 "asked-new",
				 *this, 
				 _("_Don’t save"),
				 GTK_STOCK_SAVE,
				 _("If you don’t save, the game just played will be discarded."));
			return;
		}

		/* New values */
		play_info= play_info_editor.Value();
		play_info_editor.Have_Read();

		/* Reset */
		playing= true;
		modified= false;
		Update_Playing();
		play_players[cc_w].reset();
		play_players[cc_b].reset();
		game.New(play_info.starting_position); 
		
		for (int i= 0;  i < 2;  ++i)
		{
			if (play_info.players[i].type != p_human)
				if (! New_Play_Player(play_players[i],
						      play_info.players[i],
						      i ? "black":"white",
						      *this,
						      play_info.starting_position))
				{
					Inform_Error(Format(_("Cannot start %s"),
							    play_info.players[i].name()),
						     this);
					return;
				}
		}
		for (int i= 0;  i < 2 ; ++i)
		{
			players_display.Set_Name(i, play_info.players[i].name);
			game.info.players[i]= play_info.players[i].name; 
		}
		players_display.Reset_Clocks(play_info.time); 
		

		/* Start engine */ 
		if (play_players[game.Current_Position().To_Move()].get())
		{
			play_players[game.Current_Position().To_Move()]->MP_Go();
		}

		/* Hide widgets */
		promotion_chooser.Set_Visible(false);
		material_widget.Hide();

		/* Caption */
		String title= Format("%s – %s",
				     play_info.players[cc_w].name(),
				     play_info.players[cc_b].name());
		gtk_window_set_title
			(GTK_WINDOW(Root().Get()), title());

		/* Display the side to move */
		Update_Position(); 

		/* Clear board */
		board.Invalidate_Position();
		board.Clear_Last_Move(); 

		/* Turn board */
		if ((play_info.players[cc_w].type == p_human) !=
		    (play_info.players[cc_b].type == p_human))
			board.Set_Turned(play_info.players[cc_b].type == p_human); 
		
		/* Opening */
		opening_widget.Update();

		/* Game move list */
		game_list.Update();
	}
	else if (message == s_new)
	{
		New();
	}

	else if (message == s_save)
	{
		file_dialog.Open(File_Dialog::t_save,
				 "asked_name",
				 *this);
	}

	else if (message == "asked-close"  ||
		 message == "asked-new")
	{
		int what= message == "asked-close" ? 0 : 1;
		int choice= ask_dialog.Answer(); 
		switch (choice)
		{
		default:  assert (0);  break;
		case GTK_RESPONSE_NO:
			if (what)
			{
				modified= false; 
				MR_Receive(s_play_info_edited);
			}
			else
				Close(true);
			break;
		case GTK_RESPONSE_CANCEL:
			break;
		case GTK_RESPONSE_YES:
			file_dialog.Open(File_Dialog::t_save,
					 what ? "asked_name-new" : "asked_name-close",
					 *this);
			break;
		}
		ask_dialog.Have_Read(); 
	}

	else if (message == "asked_name-new" ||
		 message == "asked_name-close" ||
		 message == "asked_name")
	{
		if (! Write_To_File(file_dialog.Filename()(),
				    game))
		{
			Inform_Error(Format(_("Saving \"%s\""),
					    file_dialog.Filename()()),
				     this);
		}
		else 
		{
			modified= false; 
			if (message == "asked_name-new")
				MR_Receive(s_play_info_edited);
			else if (message == "asked_name-close")
				Close(true); 
		}
		file_dialog.Have_Read(); 
	}

	/* user move */ 
	else if (message == "move")
	{
		if (! playing)  return; 

		int from, to;
		if (! board.Get_Squares(from, to))  return;

		/* it's an engine's turn:  don't make the move */ 
		if (play_players[game.Current_Position().To_Move()].get())
			return;

		int index= game.Current_Generator().Square_Move(from, to);
		if (index < 0)  return; /* no such move */ 

		if (game.Current_Generator()[index].promotion & pi_color)
			index += promotion_chooser.Selection();

		players_display.Set_To_Move(1^game.Current_Position().To_Move());
		board.Invalidate_Move_Squares(game.Current_Generator()[index]);
		board.Set_Last_Move(from, to);

		/* pass the move to the engine if it is now an engine’s turn */ 
		if (play_players[1^game.Current_Position().To_Move()].get())
		{
			play_players[1^game.Current_Position().To_Move()]->MP_Move
				(Simple_Notation(game.Current_Generator()[index]));
		}

		game.Append_Move(index);
		Check_End_Of_Game(); 
		Update_Last_Move();
		Update_Position(); 
		opening_widget.Update();
		game_list.Update_Length();
		game_list.Update_Content_One(game.Overall_Location()-1);
		game_list.Update_Highlight();
		modified= true;
	}

	else if (message == "game_list")
	{
	}

	else if (message == s_resign)
	{
		if (! playing)  return;

		/* check that the user is to move */
		if (play_players[game.Current_Position().To_Move()].get())
			return;

		/* set score */
		game.info.result= 1<<(1^game.Current_Position().To_Move());

		/* finish game */ 
		Update_Last_Move();
		Finish_Game();
		modified= true;
	}
	else if (message == s_take_back)
	{
		if (! playing)   return; 
		if (! Go_Backward())  return; 
		int to_move= game.Current_Position().To_Move();
		if (play_players[to_move].get())
		{
			Go_Backward();
			play_players[to_move]->MP_Take_Back();
		}
		if (play_players[1^to_move].get())
			play_players[1^to_move]->MP_Take_Back();
		game.Append_Variation(); 
		game_list.Update_Length();
		game_list.Update_Content(game.Overall_Location());
		game_list.Update_Highlight();
		int l= game.Overall_Location();
		if (l)
		{
			const Move &move= game.Overall_Move(l-1);
			board.Set_Last_Move(move.from, move.to);
		}
		else
			board.Clear_Last_Move(); 
	}

	else if (message == "turn")
	{
		material_widget.Set_Turned(game.Current_Position(), board.Turned());
		players_display.Set_Turned(board.Turned()); 
	}

	else if (message == "notation" ||
		 message == "notation_settings")
	{
		game_list.Update();
		Update_Last_Move(); 
	}
	else if (message == "edit_notation")
	{
		notation_preferences.Edit(); 
	}
	else if (message == "edit_notation_settings")
		notation_settings_preferences.Edit();
	else if (message == s_open_in_editor)
	{
		String filename= Write_To_Tmp_File(game);
		if (filename == "" || ! _GUI_Execute(*this, "--edit-game", filename(), "-a"))
		{
			Inform_Error(_("Cannot open game"), this);
			return;
		}
	}

	/* engine move */ 
	else if ((!memcmp(message(),"black ", 6))||
		 (!memcmp(message(),"white ", 6)))
	{
		int color= message[0] == 'b';
		if (color != game.Current_Position().To_Move())
		{
			/* Engines may send moves at the wrong time.*/
			Verbose("wrong time"); 
			return;
		}
		String move= message()+6;
		int move_index= Parse_Move(move, game.Current_Generator());
		if (move_index < 0)  return;
		gdk_beep(); 
		const Move &m= game.Current_Generator()[move_index];
		board.Invalidate_Move_Squares(m);
		board.Set_Last_Move(m.from, m.to);

		/* pass the move to the engine if it is now an engine’s turn */ 
		if (play_players[1^game.Current_Position().To_Move()].get())
		{
			play_players[1^game.Current_Position().To_Move()]->MP_Move
				(Simple_Notation(game.Current_Generator()[move_index]));
		}

		game.Append_Move(move_index);
		Check_End_Of_Game();
		Update_Last_Move();
		Update_Position(); 
		opening_widget.Update();
		game_list.Update_Length();
		game_list.Update_Content_One(game.Overall_Location()-1);
		game_list.Update_Highlight();
		players_display.Set_To_Move(game.Current_Position().To_Move());
		modified= true;
	}

	else if (message == s_pause)
	{
		
	}

	else
		Window::MR_Receive(message);
}

void Play_Window::Update_Last_Move()
{
	statusbar.Set(0, game.Format_Last_Move(notation_preferences(),
					       notation_settings_preferences())); 
	promotion_chooser.Update(game.Current_Position());
	menubar.Set_Sensitive("/Game/Take Back Last Move", 
			      game.Overall_Location() != 0);
}

void Play_Window::Update_Position()
{
	statusbar.Set(1, playing ? game.To_Move_Description() : "");

	material_widget.Update(game.Current_Position(),
			       board.Turned(),
			       side_box);
}

void Play_Window::Check_End_Of_Game()
{
	if (0 == *game.Current_Generator())
	{
		game.info.result= game.Current_Position().In_Check()
			? (1^game.Current_Position().To_Move())<<1
			: re_draw; 
		Finish_Game();
	}
}

void Play_Window::Finish_Game()
{
	playing= false;
	play_players[cc_w].reset();
	play_players[cc_b].reset(); 
	players_display.Set_To_Move(-1); 
//	statusbar.Set(0, statusbar.Get(0)+" "+game.info.result
	Update_Playing();
}

bool Play_Window::Wdg_Close_Query()
{
	if (modified)
	{
		ask_dialog.Open(_("Save the game before closing?"),
				"asked-close",
				*this, 
				_("Close _without saving"),
				GTK_STOCK_SAVE,
				_("If you don’t save, the game just played will be discarded."));
		return false; 
	}
	else
		return true;
}

void Play_Window::Wdg_Close()
{
	Verbose_Function(); 

	ask_dialog.Close();
	file_dialog.Close();
	play_info_editor.Close(); 
	play_players[cc_w].reset();
	play_players[cc_b].reset(); 
	notation_preferences.Close(); 
	notation_settings_preferences.Close();

	Window::Wdg_Close(); 
}

void Play_Window::Update_Playing()
{
	menubar.Set_Sensitive("/Game/Resign", playing);
	menubar.Set_Sensitive("/Game/Take Back Last Move", playing);
//	if (! playing)
//		menubar.Set_Sensitive("/Game/Take Back Last Move", false);
}

bool Play_Window::Go_Backward()
{
	if (game.Overall_Location() == 0)  return false; 
	Move move= game.Overall_Move(game.Overall_Location() - 1);
	while (game.Current_Location() == 0)
		game.Leave_Variation(); 
	game.Go_Backward(); 
	board.Invalidate_Move_Squares(move); 
	return true;
}

void Play_Window::New()
{
	play_info_editor.Open(s_play_info_edited, *this, play_info, this);
}

