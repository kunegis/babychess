#include "game_archive_editor.hh"

#include "help_menu.hh"
#include "inform.hh"

#include "../dep/strings.hh"

Game_Archive_Editor *Game_Archive_Editor::editor;

bool Game_Archive_Editor::DE_Open(Pointer <Box> parent)
{
	static GtkItemFactoryEntry menu_items[] = 
		{{ "/"N_("_File"),            NULL,         
		   NULL,          0,                   "<Branch>",    0},
		 { "/"N_("_Archive"),            NULL,         
		   NULL,          0,                   "<Branch>",    0},
		 { "/Archive/"N_("_Add Game"),  NULL,  
		   MENU_CALLBACK,  str_add,  "<StockItem>", GTK_STOCK_ADD},
		 { "/Archive/"N_("_Remove Game"),  NULL,  
		   MENU_CALLBACK,  str_remove,  "<StockItem>", GTK_STOCK_REMOVE},
		 { "/Archive/"N_("_Edit Game"),    NULL,  
		   MENU_CALLBACK,  str_edit,    NULL, 0},
		 { "/Archive/sep1",  NULL, 
		   NULL, 0, "<Separator>", 0},
		 { "/Archive/"N_("I_nsert File..."), NULL, 
		   MENU_CALLBACK, str_insert_file, NULL, 0},
		 HELP_MENU
		};

	if (! menubar.Open(parent(), 
			    sizeof (menu_items) / sizeof (GtkItemFactoryEntry),
			    menu_items,
			    *this))
		return false;

	Document_Editor <Game_Archive> ::Append_Menu(menubar);

	if (! toolbar.Open(parent()))
		return false;

	Document_Editor <Game_Archive> ::Append_Tool_Buttons(toolbar);

	toolbar.Append_Space();
	add_button= toolbar.Append(GTK_STOCK_ADD,
				   "add",
				   _("Add a new game"));
	remove_button= toolbar.Append(GTK_STOCK_REMOVE,
				      "remove",
				      _("Remove the selected game"));
	edit_button=   toolbar.Append_Text(_("Edit"),
					   "edit",
					   _("Edit the selected game"));

	Vector <String> titles;
	if (! titles.Set(8))
		return false;
	titles[0]= _("#");
	titles[1]= _("Result");
	titles[2]= I_("player:White");
	titles[3]= I_("player:Black"); 
	titles[4]= _("Event");
	titles[5]= _("Round");
	titles[6]= _("Date");
	titles[7]= _("Site");

	parent->Set_Next(true);
	if (! game_list.Open(parent(),
			     List::Open_Info
			     (8,
			      scr_both,
			      "list_clicked",
			      titles)))
		return false; 
	game_list->Set_Column_Right_Aligned(0);
	game_list->Set_Column_Right_Aligned(5);
	game_list->Set_Column_Right_Aligned(6);
	gtk_signal_connect(GTK_OBJECT(game_list()().Get()),
			   "click-column",
			   GTK_SIGNAL_FUNC(& Signal_Click_Column),
			   this);
	

	if (! statusbar.Open(parent(), 2))
		return false;
//	statusbar.Set(1, _("Modified"));
			
	Update_List();
	Update_Count();
	Update_Full(false);

	if (*game_archive == 1)
		/* If the archive contains exactly one game, open it
		 * immediately.  */
	{
		Edit(0); 
	}
	
	return true; 
}

void Game_Archive_Editor::EW_Set_Value(Pointer <const Game_Archive> ga)
{
	if (ga)  game_archive= ga();
}

const Game_Archive &
Game_Archive_Editor::EW_Get_Value()
{
	return game_archive;
}

bool Game_Archive_Editor::EW_Change_Value(const Game_Archive &new_value)
{
	Close_Editors();
	game_archive= new_value;
	Invalidate_Order();
	Update_List();
	Update_Count();
	return true;
}

void Game_Archive_Editor::Update_List(int from,
				      int new_sort_index)
{
	assert (new_sort_index < 7);

	if (! game_list.Is()) return;
	
	game_list->Set_Row_Count(*game_archive); 

	sort_index= new_sort_index; 

	/* create the sort order */ 
	if (new_sort_index >= 0 && ! sort_order[new_sort_index].Is())
	{
		if (! sort_order [new_sort_index].Set(*game_archive)) return; 
		if (! sort_iorder[new_sort_index].Set(*game_archive)) return;

		for (int i= 0;  i < *game_archive;  ++i)
			sort_order[new_sort_index][i]= i;
		editor= this;
		qsort(sort_order[sort_index](0), 
		      *game_archive,
		      sizeof (int),
		      & Compare);
		for (int i= 0;  i < *game_archive;  ++i)
			sort_iorder[sort_index][sort_order[new_sort_index][i]]= i;
	}

	for (int i= from;  i < *game_archive;  ++i)
	{
		Update_Game_Info(i, Game_Index(i));
	}
}

int Game_Archive_Editor::Compare(const void *a, const void *b)
{
	int aa= *(int*)a, bb= *(int*)b;
	return editor->game_archive.Compare(aa, bb, editor->sort_index);
}

void Game_Archive_Editor::Update_Count()
{
	String text= Format(ngettext("%d game", "%d games", *game_archive), *game_archive);
	statusbar.Set(0, text);
}

void Game_Archive_Editor::MR_Receive(String message)
{
	if (!memcmp(message(), "edited ", 7))
	{
		int editor_index;
		if (1 != sscanf(message() + 7, "%d", & editor_index))     return;
		if (editor_index < 0 || editor_index >= *edited_games)      return;
		int game_index= edited_games[editor_index].game_index;
		game_archive.Set(game_index,
				 edited_games[editor_index].game_editor.Value());
		bool have_read= edited_games[editor_index].game_editor.Have_Read();

		int list_index= List_Index(game_index);
		Update_Game_Info(list_index, game_index);
		game_list.Set_Selection(list_index);
		Modify(); 
		if (have_read)
		{
			edited_games[editor_index].game_index= -1;
			int i= *edited_games - 1;  
			while (i >= 0 && 
			       edited_games[i].game_index == -1  &&
			       ! edited_games[i].game_editor.Is())
				--i;
			edited_games.Set(i + 1);
		}
	}
	else if (message == "add")
	{
		int game_index= game_list->Selection();
		if (game_index == -1)
			game_index= *game_archive;
		if (! game_archive.Add_Game(game_index))
			return;
		for (int i= 0;  i < *edited_games;  ++i)
		{
			if (edited_games[i].game_index >= game_index)
				++ edited_games[i].game_index;
		}
		Update_Count();
		Invalidate_Order();
		Update_List();
		game_list.Set_Selection(game_index);
		Update_Full(true);

		Modify(); 

		for (int i= 0;  i < *edited_games;  ++i)
		{
			if (edited_games[i].game_index > game_index)
				Update_Edited_Game_Caption(i);
		}
	}
	else if (message == "remove")
	{
		int game_index= game_list->Selection();
		if (game_index < 0)  return;

		/* Close child editor if the game is being edited. 
		 */
		int found= -1;
		for (int i= 0;  i < *edited_games;  ++i)
		{
			if (game_index == edited_games[i].game_index)
			{
				found= i;
				edited_games[i].game_editor.Close();
			}
			else if (edited_games[i].game_index > game_index)
				-- edited_games[i].game_index;
		}
		if (found >= 0)
		{
			if (found + 1 < *edited_games)
			{
				edited_games[found].game_index= edited_games[-1].game_index;
				edited_games[found].game_editor << edited_games[-1].game_editor; 
			}
			edited_games= -1;
		}

		game_archive.Remove(game_index);

		Update_Count();
		Invalidate_Order();
		Update_List();
		if (*game_archive != 0)
		{
			game_list.Set_Selection(game_index == *game_archive
						? game_index - 1
						: game_index);
		}
		else
		{
			Update_Full(false);
		}
		for (int i= game_index;  i < *game_archive;  ++i)
			Update_Edited_Game_Caption(i);
		Modify(); 
	}
	else if (message == "edit")
	{
		Edit();
	}
	else if (message == "list_clicked")
	{
		if (*game_archive == 0)  return;

		int r, c;
		game_list->Clicked(r, c); 
		game_list.Set_Selection(r); 
		Update_Full(true);
		if (game_list->Double_Click())
			Edit(-1, false);
	}
	else if (message == "insert_file")
		file_dialog.Open(File_Dialog::t_insert,
				 "filename_edited", *this); 
	else if (message == "filename_edited")
	{
		String filename= file_dialog.Filename();
		file_dialog.Have_Read();
		Game_Archive a;
		if (! (Read_From_File(filename(), a)  &&
		       game_archive.Transfer_From(a)))
		{
			Inform_Error(Format(_("Cannot insert \"%s\""), filename()),
				     this);
			return;
		}
		Invalidate_Order();
		Update_List(); 
		Update_Count();
	}
	else 
		Document_Editor_Base::MR_Receive(message);
}

void Game_Archive_Editor::Wdg_Close()
{
	Close_Editors();
	Document_Editor <Game_Archive> ::Wdg_Close();
}

void Game_Archive_Editor::Close_Editors()
{
	for (int i= 0; i < *edited_games; ++i)
	{
		edited_games[i].game_editor.Close();
	}
	edited_games.Free();
}

void Game_Archive_Editor::Edit(int game_index,
			       bool set_selection)
{
	/* Determine which game to edit. */
	assert (game_index >= -1); 
	if (game_index < 0)
	{
		int list_index= game_list->Selection();
		if (list_index < 0)  return;
		game_index= Game_Index(list_index);
	}
	else
	{
		if (set_selection)  game_list.Set_Selection(game_index); 
	}

	/* Check whether game is already being edited. */
	for (int i= 0;  i < *edited_games;  ++i)
	{
		if (edited_games[i].game_index == game_index)
		{
			if (! game_archive.Check(game_index))  return; 
			edited_games[i].game_editor.Open
				(Format("edited %d", i),
				 *this,
				 game_archive[game_index],
				 this);
			return;
		}
	}

	/* Open new editor */
	if (! game_archive.Check(game_index))  return;
	int size= *edited_games;
	if (! edited_games.Set(size + 1))  return;
	edited_games[size].game_index= game_index;
	edited_games[size].game_editor.Open
		(Format("edited %d", size),
		 *this,
		 game_archive[game_index],
		 this,
		 Get_Game_Editor_Caption(size));
}

void Game_Archive_Editor::DE_Reset()
{
	game_archive.Clear();

	Invalidate_Order();
	Update_List();
	Update_Count();
}

void Game_Archive_Editor::Update_Game_Info(int list_index,
					   int game_index)
{
	if (game_index < 0)  game_index= list_index;

	game_list->Set(0, list_index, Format("%d", game_index));
	game_list->Set(1, list_index, game_archive.Get_Readable_Result(game_index));
	game_list->Set(2, list_index, game_archive.Get_White(game_index));
	game_list->Set(3, list_index, game_archive.Get_Black(game_index));
	game_list->Set(4, list_index, game_archive.Get_Event(game_index));
	game_list->Set(5, list_index, game_archive.Get_Round(game_index));
	game_list->Set(6, list_index, game_archive.Get_Date_Locally(game_index));
	game_list->Set(7, list_index, game_archive.Get_Site(game_index));
}

void Game_Archive_Editor::DO_Modified_Changed(int modified)
{
	statusbar.Set(1, modified>=2 ? _("Modified") : "");
//	statusbar.Set_Sensitive(1, modified);
}

void Game_Archive_Editor::Update_Edited_Game_Caption(int editor_index)
{
	edited_games[editor_index].game_editor().Set_Caption(Get_Game_Editor_Caption(editor_index));
}

String Game_Archive_Editor::Get_Game_Editor_Caption(int editor_index)
{
	return Format("%s %d", _("Game"), edited_games[editor_index].game_index);
}

void Game_Archive_Editor::EW_Caption_Changed()
{
	for (int i= 0;  i < *edited_games;  ++i)
		edited_games[i].game_editor.Update_Caption(); 
}

void Game_Archive_Editor::Update_Full(bool full)
{
	menubar.Set_Sensitive("/Archive/Remove Game", full);
	menubar.Set_Sensitive("/Archive/Edit Game", full); 
	gtk_widget_set_sensitive(remove_button, full);
	gtk_widget_set_sensitive(edit_button, full);
}

void Game_Archive_Editor::Signal_Click_Column(GtkCList * /* clist */ ,
					      gint column,
					      gpointer editor)
{
	static int ord[7]= {6, 4, 5, 0, 3, 2, 1};
	Game_Archive_Editor *a= (Game_Archive_Editor *) editor;
	int new_sort_index= column ? ord[column-1] : -1;
	if (new_sort_index == a->sort_index)  return;
	a->Update_List(0, new_sort_index);
}

	void Game_Archive_Editor::Invalidate_Order() 
		{
			for (int i= 0;  i < 7;  ++i)
			{
				sort_order[i]= sort_iorder[i]= 0;
			}
		}

	int Game_Archive_Editor::List_Index(int game_index) const
		{ return sort_index >= 0 ? sort_iorder[sort_index][game_index] : game_index;  }
	int Game_Archive_Editor::Game_Index(int list_index) const
		{ return sort_index >= 0 ? sort_order [sort_index][list_index] : list_index;  }
