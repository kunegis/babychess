#include "position_editor.hh"

#include "piece_name.hh"
#include "fen.hh"
#include "help_menu.hh"
#include "gui_execute.hh"
#include "inform.hh"
#include "error.hh"

#include "../dep/strings.hh"

#include <gtk/gtktogglebutton.h>

bool Position_Editor::next_turned= false;

bool Position_Editor::DE_Open(Pointer <Box> parent)
{
	Verbose_Function();

	notation_preferences.Open((*this), "notation");
	notation_settings_preferences.Open(*this, "notation_settings");

	Normalize_Castlings();

	/* menubar */ 
	static GtkItemFactoryEntry menu_items[] = 
		{{ "/"N_("_File"),            NULL,         
		   NULL,          0,                   "<Branch>",    0},
		 { "/"N_("_View"),            NULL,         
		   NULL,          0,                   "<Branch>",    0},
		 { "/View/"N_("_Legal Moves"), "<ctrl>l",
		   MENU_CALLBACK, str_show_legal,      "<CheckItem>", 0},
		 { "/View/"N_("_FEN Notation"),NULL,         
		   MENU_CALLBACK, str_show_fen,        "<CheckItem>", 0},
		 { "/View/"N_("Hide the Side _Bar"),    NULL,         
		   MENU_CALLBACK, str_hide_side_bar,   "<CheckItem>", 0},
		 { "/View/sep1",                 NULL,         
		   NULL,          0,                   "<Separator>", 0},
		 { "/View/"N_("_Choose Notation"),NULL,
		   MENU_CALLBACK, str_edit_notation, "<StockItem>",  GTK_STOCK_PREFERENCES},
		 { "/View/"N_("_Notation Preferences"),   NULL, 
		  MENU_CALLBACK,str_edit_notation_settings,"<StockItem>",GTK_STOCK_PREFERENCES},
		 { "/"N_("_Position"),            NULL,         
		   NULL,          0,                   "<Branch>",    0},
		 { "/Position/"/*the verb to empty */N_("_Empty"), NULL,         
		   MENU_CALLBACK, str_empty,           "<StockItem>",       GTK_STOCK_REMOVE},
		 { "/Position/"N_("_Invert"), NULL,     
		   MENU_CALLBACK, str_invert,            NULL,          0},
		 { "/Position/"N_("_Mirror"), NULL,     
		   MENU_CALLBACK, str_mirror,       NULL,          0},
		 { "/Position/sep1",                 NULL,         
		   NULL,          0,                   "<Separator>", 0},
		 { "/Position/"N_("New _Game From This Position"),    NULL,         
		   MENU_CALLBACK, str_new_game,        NULL,          0},
		 BOARD_MENU
		 HELP_MENU
		};
	if (! menubar.Open(parent(), sizeof(menu_items)/sizeof(menu_items[0]),
			    menu_items, *this))
		return false;

	Document_Editor <Position> ::Append_Menu(menubar);

	/* toolbar */ 
	if (! toolbar.Open(parent()))  return false;
	
	Document_Editor <Position> ::Append_Tool_Buttons(toolbar);
	toolbar.Append_Space();
	toolbar.Append(GTK_STOCK_REMOVE,
		       s_empty,
		       _("Remove all pieces"));
	/* middle */ 
	parent->Set_Next(true); 
	if (! middle_box.Open(parent()))  return false;

	if (! side_box.Open(middle_box))  return false;

	/* Color */ 
	if (! color_chooser.Open(side_box,   "color", _("To move"),  2))
		return false; 

	color_chooser() << I_("to move:_White")
			<< I_("to move:_Black");
	color_chooser->Set_Selection(position.To_Move());

	/* Last move */ 
	if (! last_move_box.Open(side_box, dialog_unit/2))  return false;
	last_move_label.Open(last_move_box, _("_Last move:"),
			     0.0, "", true);
	
	last_move_box.Set_Next(true); 
	last_move_menu.Open(last_move_box, "ep"); 
	gtk_label_set_mnemonic_widget(GTK_LABEL(last_move_label.Get()), 
				      last_move_menu.Get());
	gtk_widget_set_sensitive(last_move_label.Get(), false);
	gtk_widget_set_sensitive(last_move_menu.Get(), false);
	
	/* Castlings */ 
	if (! castling_label.Open(side_box, _("Possible castlings")))
		return false;
	if (! castling_box.Open(side_box))
//				   Frame::Open_Info(GTK_SHADOW_IN,
//						    _("Possible castlings")),
//				   Table::Open_Info(2)))
		return false;

	for (int i= 0;  i < 4;  ++i)
	{
		if (! castling_check_boxes[i].Open
		    (castling_box,
		     false,
		     String('_')+
		     String(notation_preferences->texts[i_color + i/2])+' '+
		     notation_preferences->texts[i_castling + i%2],
		     Format("castling %d", i)))
			return false;
	}

	/* Previous moves */ 
	Spin_Button::Open_Info oi= {"pm", 1, 1, 10000};
	if (! previous_moves.Open(side_box, 
				  _("_First move number"),
				  oi))  
		return false;

	/* Halfmove clock */ 
	if (! hm_box.Open(side_box))  return false;
	hm_box.Set_Next(true); 
	if (! hm_text.Open(hm_box, _("_Halfmove clock:"),
			   0.0, "", true))  return false;
	Spin_Button::Open_Info hm_oi= {"hm", 0, 0, 100};
	if (! hm_edit.Open(hm_box, hm_oi))  return false;
	gtk_label_set_mnemonic_widget(GTK_LABEL(hm_text.Get()), hm_edit.Get()); 

	/* Type */ 
	if (! type_label.Open(side_box, _("Add piece")))  return false;
	for (int i= 0;  i < 3;  ++i)
	{
		if (!type_boxes[i].Open(side_box)) 
			return false;
		for (int j= 0;  j < 4;  ++j)
		{
			int piece= Piece(j/2, j%2+2*i);
			if (! type_buttons[4*i+j].Open
			    (type_boxes[i],
			     piece,
			     "",
			     (i || j) ? &type_buttons[0]:(Widget*)NULL))
				return false;
			gtk_toggle_button_set_mode
				(GTK_TOGGLE_BUTTON(type_buttons[4*i+j].Get()), FALSE); 
		}
	}
	
	/* Board */ 
	middle_box.Set_Next(true); 
	bool nturned= next_turned;
	next_turned= false;
	if (! board.Open(middle_box,
			 Board::Open_Info(position,
					  "dragged",
					  "remove",
					  "turn", "", "",
					  NULL, NULL,
					  nturned,
					  menubar)))
		return false;

	/* FEN */ 
	if (! fen_box.Open(parent()))
		return false;
	fen_box.Set_Visible(false);
	if (! fen_text.Open(fen_box, Format("FEN%s", _(":"))))
		return false;
	fen_box.Set_Next(true);
	if (! fen_entry.Open(fen_box, Entry::Open_Info("fen")))
		return false;
	if (! fen_button.Open(fen_box, 
			      "fen",
			      _("Set")))
		return false;

	/* status bar */ 
	if (! statusbar.Open(parent(), 2))  return false; 
	board.statusbar= statusbar.Get(0);

	Update_Board();

	Verbose("end position_editor::open");

	return true; 
}

void Position_Editor::MR_Receive(String message)
{
	if (!memcmp(message(), "board_", 6))
		board.MR_Receive(message);
	else if (!memcmp(message(), "castling ", 9))
	{
		int index;
		if (1 != sscanf(message() + 9, "%d", &index))  return;
		if (index < 0 || index >= 4)  return;
		Verbose_Int(index);
		assert (index >= 0 && index < 4);
		int moved= castling_check_boxes[index].Checked() ? 0 : 1;
		Verbose_Int(moved); 
		int mindex= Castling_Moved(index);
		Verbose_Int(mindex);
		position.Set_Moved(mindex, moved);
		/* legality doesn't change. */
		/* EP doesn't change. */
		Update_FEN();
		Update_Legal();
		Modify();
	}
	else if (message == "invert")
	{
		position.Invert();
		position.Current_Material().Invert(); 
		Update_Board();
		Update_EP();
		Update_FEN();
		Update_Castlings();
		Update_Legal();
		Update_Material();
		Modify(); 
	}
	else if (message == "mirror")
	{
		position.Mirror();
		Update_Board();
		Update_EP();
		Update_FEN();
		Update_Castlings();
		Update_Legal();
		Modify();
	}
	else if (message == "color")
	{
		int new_to_move= 
			color_chooser->Selection() == 0
			? cc_w : cc_b; 
		if (position.To_Move() == new_to_move)  return;
		position.Change_To_Move();
		Update_Legality();
		Update_EP();
		Update_FEN();
		Update_Legal();
		Modify();
	}
	else if (message == "new_game")
		_GUI_Execute(NULL, "--edit-game", "-p", FEN(position)());
	else if (message == "dragged")
	{
		int from, to;
		if (board.Get_Squares(from, to))
		{
			if (position[from] & pi_color)    /* dragged a piece */
			{
				if (to == 0) /* drag out of the board */
				{
					Remove_Piece(from);
				}
				else if (to != from)
				{
					position.Transfer_Piece(from, to);
					Update_Legality(); 
					Update_EP();
					Update_FEN();
					Update_Castlings();
					Update_Legal();
					Update_Material();
					/* because a piece may have been
					 * removed */ 
					Modify();
					board.Invalidate_Square(from, false);
					board.Invalidate_Square(to,   false); 
				}
			}
			else /* add new piece */
			{
				Insert_New_Piece(from);
			}
		}
	}
	else if (message == "remove")
	{
		int from, to;
		if (! board.Get_Squares(from, to))
			return;
		Remove_Piece(from);

	}
	else if (message == "notation" ||
		 message == "notation_settings")
	{
		Update_Legal(); 

		/* update castling chooser */
		for (int i= 0;  i < 4;  ++i)
		{
			String text= Format("%s%s",
					    notation_preferences->texts[i_color + i/2](),
					    notation_preferences->texts[i_castling + i%2]());
			gtk_button_set_label(GTK_BUTTON(castling_check_boxes[i].Get()),
					     text());
		}
	}
	else if (message == "fen")
	{
		Position new_position;
		if (! Parse_FEN(fen_entry.Text(), new_position))
		{
			Inform_Error(_("FEN error"), *this);
			return;
		}
		position= new_position;
		position.Set_Not_Legal(); 
		assert (!position.Legal()); 
		Update_Board();
		Modify(); 
	}

	else if (message == "ep")
	{
		int selection= last_move_menu.Selection();
		if (selection < 0)  return;
		if (selection == last_move_menu.Count() - 1)
		{
			position.EP_Poss()= 0;
		}
		else
		{
			assert (selection < last_move_menu.Count());
			int file= ep_index[selection];
			position.EP_Poss()= Make_Square
				(file, position.To_Move() == cc_w ? sq_5 : sq_4);
		}
		Verbose_Int(position.EP_Poss());

#ifndef NDEBUG		
		assert (legal); 
		Update_Legality();
		assert (legal);
		/* If the ep chooser could be clicked, then the
		 * position was legal, and the new position
		 * with another last move must also be legal. 
		 */
#endif /* ! NDEBUG */ 

		Update_FEN();
		Update_Legal();
		Modify();
	}

	else if (message == "show_legal")
	{
		if (! legal_move_list.Is())
		{
			if (! legal_info.Open(side_box))   return;
			
			side_box.Set_Next(true);
			if (! legal_move_list.Open(side_box))   return;
		}
		else
		{
			bool visible= ! GTK_WIDGET_VISIBLE(legal_move_list.Get());
			legal_info.Set_Visible(visible);
			legal_move_list.Set_Visible(visible);
		}
		if (GTK_WIDGET_VISIBLE(legal_move_list.Get()))
			side_box.Set_Visible(true);
		Update_Legal();
	}
	else if (message == "show_fen")
	{
		fen_box.Set_Visible(! GTK_WIDGET_VISIBLE(fen_box.Get())); 
		if (GTK_WIDGET_VISIBLE(fen_box.Get()))
			side_box.Set_Visible(true); 
	}
	else if (message == "hide_side_bar")
	{
		side_box.Set_Visible(! menubar.Active("/View/Hide the side bar")); 
	}
	else if (message == "hm")
	{
		position.Set_Halfmove_Clock(hm_edit.Value()); 
		Update_FEN(); 
		Modify();
	}
	else if (message == "pm")
	{
		Retrieve_PM(); 
		Update_FEN();
		Modify(); 
	}
	else if (message == "edit_notation")
	{
		notation_preferences.Edit(); 
	}
	else if (message == "edit_notation_settings")
		notation_settings_preferences.Edit();
	else if (message == "empty")
	{
		position.Empty();
		position.Set_Moved(0, 0);
		position.Set_Moved(2, 0);
		position.Set_Moved(3, 0);
		position.Set_Moved(5, 0);
		Update_Board(); 
		Modify();
	}
	else if (message == "turn")
	{
		material_widget.Set_Turned(position, board.Turned());
	}

	else 
		Document_Editor <Position> ::MR_Receive(message);
}

int Position_Editor::EW_Use()
{
	return legal ? use_ok : use_retry;
}

void Position_Editor::Update_Board()
{
	board.Invalidate_Position();
	color_chooser->Set_Selection(position.To_Move()); 
	Update_Legality(); 
	Update_EP();
	Update_FEN();
	Update_Castlings();
	Update_PM();
	Update_HM();
	Update_Legal();
	Update_Material();
}

void Position_Editor::EW_Set_Value(Pointer <const Position> value)
{
	if (!value)
		position.Reset();
	else
	{
		assert (value().Legal());
		position= value();
	}
	position.Set_Not_Legal();
	Normalize_Castlings();
	position.Update_Material(); 
}

const Position &Position_Editor::EW_Get_Value() 
{
	if (Is())
		Retrieve_PM();
	
	if (position[sq_a1] != pi_wR)
		position.Set_Moved(0, 1);
	if (position[sq_e1] != pi_wK)
		position.Set_Moved(1, 1);
	if (position[sq_h1] != pi_wR)
		position.Set_Moved(2, 1);
	if (position[sq_a8] != pi_bR)
		position.Set_Moved(3, 1);
	if (position[sq_e8] != pi_bK)
		position.Set_Moved(4, 1);
	if (position[sq_h8] != pi_bR)
		position.Set_Moved(5, 1);

	return position;
}

bool Position_Editor::EW_Change_Value(const Position &p)
{
	position= p;
	position.Update_Material();
	Update_Board();
	return true;
}

void Position_Editor::DE_Reset()
{
	position.Reset();
	position.Set_Not_Legal(); 
	Update_Board();
}

void Position_Editor::Update_Legality()
{
	Verbose_Function();

	int ep_square= position.EP_Poss();
	position.EP_Poss()= 0;
	if (position.Would_Be_Legal())
	{
		statusbar.Set(0, _("Position is legal"));
		legal= true;
	}
	else
	{
		statusbar.Set(0, Error::Get());
		legal= false;
	}
	position.EP_Poss()= ep_square;
	menubar.Set_Sensitive("/Position/New Game From This Position", legal); 
	Set_Valid(legal, menubar);
}

void Position_Editor::Update_FEN()
{
	if (! legal)
	{
		fen_entry.Set("");
		return;
	}
	fen_entry.Set(FEN(position));
}

void Position_Editor::Update_EP()
{
	last_move_menu.Clear();
	if (! legal)
	{
		gtk_widget_set_sensitive(last_move_label.Get(), false);
		gtk_widget_set_sensitive(last_move_menu.Get(), false);
		return;
	}

	int ep_files= 0;
	/* bitmask of files on which there could have been double pawn
	 * moves. */

	int to_move= position.To_Move();
	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		if (position[Make_Square(file, to_move == cc_w ? sq_5 : sq_4)]
		    == Piece(1^to_move, pi_P)   
		    && position[Make_Square(file, to_move == cc_w ? sq_6 : sq_3)] == pi_empty
		    && position[Make_Square(file, to_move == cc_w ? sq_7 : sq_2)] == pi_empty
		    && (position[Make_Invalid_Square(file + 1, to_move == cc_w ? sq_5 : sq_4)]
			== Piece(to_move, pi_P)        
			|| position[Make_Invalid_Square(file - 1, to_move == cc_w ? sq_5 : sq_4)]
			== Piece(to_move, pi_P)))
		{
			/* Now check whether a king capture would have
			 * been possible before the double pawn move.
			 */
			int ep_square= Make_Square(file, to_move == cc_w ? sq_5 : sq_4);
			int home_square= Make_Square(file, to_move == cc_w ? sq_7 : sq_2);
			position[home_square]= position[ep_square];
			position[ep_square]= pi_empty;
			if (! position.Attacked_From(ep_square))
				ep_files |= (1 << file);
			position[ep_square]= position[home_square];
			position[home_square]= pi_empty; 
		}
	}
	if (! ep_files)
	{
		gtk_widget_set_sensitive(last_move_label.Get(), false);
		gtk_widget_set_sensitive(last_move_menu.Get(), false);
		position.EP_Poss()= 0;
		return;
	}
	int select_index= -1;

	char from_rank= to_move == cc_w ? '7' : '2';
	char to_rank=   to_move == cc_w ? '5' : '4';
	int ep_index_index= 0;
	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		if (ep_files & (1 << file))
		{
			ep_index[ep_index_index++]= file;
			char f= file - sq_a + 'a';
			last_move_menu << 
				Format("%c%c–%c%c",
				       f,
				       from_rank,
				       f,
				       to_rank);
			if (position.EP_Poss() &&
			    file == File(position.EP_Poss()))
			{
				select_index= ep_index_index-1;
			}
		}
	}

	Verbose_Int(select_index);
	last_move_menu << _("Another move");
	last_move_menu.Update();
	if (select_index == -1)
	{
		select_index= last_move_menu.Count() - 1;
		position.EP_Poss()= 0;
	}
	last_move_menu.Set_Selection(select_index);
	gtk_widget_set_sensitive(last_move_label.Get(), true);
	gtk_widget_set_sensitive(last_move_menu.Get(), true);
}

void Position_Editor::Insert_New_Piece(int square)
{
//	int s= new_type->Selection();
	int s= -1;
	for (int i= 0;  i < 12;  ++i)
	{
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(type_buttons[i].Get())))
		{
			s= i;
			break;
		}
	}

	if (s < 0)  return;

//	int new_piece= Piece(1 - s / 6, s % 6);
	int new_piece= Piece(s%4/2, s%2+(s/2&~1));

	position.Insert_Piece(square, new_piece);

	Update_Legality();
	Update_EP();
	Update_FEN();
	Update_Castlings();
	Update_Legal();
	Update_Material();
	Modify();
	board.Invalidate_Square(square, false); 
}

bool Position_Editor::EW_Try_Load(String fen, 
				  Pointer <Position> position)
{
	return Parse_FEN(fen, position());
}

void Position_Editor::Update_Legal()
{
	if (! legal_move_list.Is()) return; 
	if (! legal_info.Is())      return;

	if (! GTK_WIDGET_VISIBLE(legal_move_list.Get()))   return;
	if (! GTK_WIDGET_VISIBLE(legal_info.Get()))        return; 

	if (! legal)
	{
		legal_move_list.Clear();
		return;
	}
	
	position.Update_Material();
	assert (position.Moved(1) == 0);
	assert (position.Moved(4) == 0); 
	int a1= position.Moved(0), h1= position.Moved(2);
	int a8= position.Moved(3), h8= position.Moved(5);
	if (position[sq_a1] != pi_wR)  position.Set_Moved(0, 1);
	if (position[sq_h1] != pi_wR)  position.Set_Moved(2, 1);
	if (position[sq_a8] != pi_bR)  position.Set_Moved(3, 1);
	if (position[sq_h8] != pi_bR)  position.Set_Moved(5, 1);
	bool l= position.Set_Legal();
	assert (l); 
	Generator g;
	g.New(position);
	
	legal_info.Set(g, notation_preferences());
	legal_move_list.Set(g, notation_preferences(), notation_settings_preferences());

	position.Set_Not_Legal(); 
	position.Set_Moved(0, a1);
	position.Set_Moved(2, h1);
	position.Set_Moved(3, a8);
	position.Set_Moved(5, h8);
}

void Position_Editor::Update_Castlings()
{
	const int squares[4][2]
		= {{sq_e1, sq_h1},
		   {sq_e1, sq_a1},
		   {sq_e8, sq_h8},
		   {sq_e8, sq_a8}};
	
	for (int i= 0;  i < 4;  ++i)
	{
		gtk_widget_set_sensitive(castling_check_boxes[i].Get(),
					 position[squares[i][0]] == Piece(i / 2, pi_K) &&
					 position[squares[i][1]] == Piece(i / 2, pi_R));
		int moved_index= Castling_Moved(i);
		castling_check_boxes[i].Set_Checked
			(position.Moved(moved_index) == 0);
	}
}

void Position_Editor::Update_PM()
{
	previous_moves.button.Set(position.previous_move_count / 2 + 1);
}

void Position_Editor::Wdg_Close()
{
	Retrieve_PM();
	notation_preferences.Close(); 
	notation_settings_preferences.Close(); 
	Document_Editor <Position> ::Wdg_Close(); 
}

void Position_Editor::Retrieve_PM()
{
	position.previous_move_count= 
		(previous_moves.button.Value() - 1) * 2
		+ position.To_Move();
}

void Position_Editor::Remove_Piece(int square)
{
	int piece= position[square];
	if (piece == pi_empty)  return;
	position.Remove_Piece(square);
	-- position.Current_Material()(piece);
	board.Invalidate_Square(square, false);
	Update_Legality();
	Update_EP();
	Update_FEN();
	Update_Castlings();
	Update_Legal();
	Update_Material();
	Modify();
}

void Position_Editor::Normalize_Castlings()
{
	if (position.Moved(1) != 0)
	{
		position.Set_Moved(0, 1);
		position.Set_Moved(1, 0);
		position.Set_Moved(2, 1);
	}

	if (position.Moved(4) != 0)
	{
		position.Set_Moved(3, 1);
		position.Set_Moved(4, 0);
		position.Set_Moved(5, 1);
	}
}

void Position_Editor::Update_HM()
{
	hm_edit.Set(position.Halfmove_Clock()); 
}

void Position_Editor::DO_Modified_Changed(int modified)
{
	statusbar.Set(1, modified>=2 ? _("Modified") : "");
}

void Position_Editor::EW_Caption_Changed()
{
	notation_preferences.Caption_Changed(); 
	notation_settings_preferences.Caption_Changed(); 
	board.Caption_Changed();
}

void Position_Editor::Update_Material()
{
	material_widget.Update(position, board.Turned(), side_box);
}
