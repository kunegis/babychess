#include "internet_window.hh"

#include "inform.hh"
#include "error.hh"
#include "settings.hh"
#include "generator.hh"
#include "parse_move.hh"
#include "about.hh"
#include "internet_account_stream.hh"
#include "notation.hh"
#include "gettext.hh"
#include "help_menu.hh"
#include "settings.hh"
#include "format_move_name.hh"

#include "../dep/strings.hh"

#include <gtk/gtkstock.h>

bool Internet_Window::Open(Open_Info)
{
	if (! Window::Open(_("Internet Chess"), 
			   (GdkPoint){20*dialog_unit*5/3,
			   17*dialog_unit*5/3}))   
		return false;

	if (! overall_box.Open(this))  goto close; 

	static GtkItemFactoryEntry menu_items[] = 
		{{ "/"N_("_Connection"),            NULL,
		   NULL,          0,                   "<Branch>",    0},
		 { "/Connection/"N_("Log_in"),      "<control>i", 
		   MENU_CALLBACK, str_login,           NULL,          0},
		 { "/Connection/"N_("Log_out"),     NULL, 
		   MENU_CALLBACK, str_logout,          NULL,          0},
		 { "/Connection/sep1",            NULL,         
		   NULL,          0,                   "<Separator>", 0},
		 { "/Connection/"N_("_Quit"),       NULL, 
		   MENU_CALLBACK, str_ok,    "<StockItem>", GTK_STOCK_QUIT},
		 
		 { "/"N_("_View"),            NULL,         
		   NULL,          0,                   "<Branch>",    0},
		 { "/View/"N_("_Messages"),   NULL, 
		   MENU_CALLBACK, str_interaction,     "<CheckItem>", 0},
		 { "/View/"N_("_Legal Moves"),"<ctrl>l", 
		   MENU_CALLBACK, str_show_legal, "<CheckItem>", 0},
		 { "/View/sep1",                 NULL,         
		   NULL,          0,                   "<Separator>", 0},
		 { "/View/"N_("_Choose Notation"),NULL,         
		   MENU_CALLBACK,str_notation,  "<StockItem>", GTK_STOCK_PREFERENCES},
		 { "/View/"N_("_Notation Preferences"),   NULL, 
		   MENU_CALLBACK, str_edit_notation_settings,"<StockItem>",GTK_STOCK_PREFERENCES},
		 BOARD_MENU
		 { "/"N_("_Game"),            NULL,         
		   NULL,           0,                  "<Branch>",    0},
		 { "/Game/"N_("_Seek"),       "<control>K", 
		   MENU_CALLBACK,  str_seek,           NULL,          0},
		 { "/Game/"N_("_Resign"),     "<control>R", 
		   MENU_CALLBACK,  str_resign,         NULL,          0},
		 HELP_MENU
		};

	menubar.Open(overall_box, 
		     sizeof (menu_items) / sizeof (GtkItemFactoryEntry),
		     menu_items,
		     *this);

	if (! toolbar.Open(overall_box))  goto close; 
	login_button= toolbar.Append_Text(_("Login"), "login",
					  _("Log into an internet chess server"));
	logout_button= toolbar.Append(_("Logout"), "logout",
				      _("Log out of the internet chess server"));

	overall_box.Set_Next(true);
	if (! top_box.Open(overall_box))     goto close;
	if (! side_box.Open(top_box)) goto close;
	if (! players_display.Open(side_box))  goto close; 
	notation_preferences.Open((*this), "notation_changed");
	notation_settings_preferences.Open((*this), "notation_settings_changed");
	if (! promotion_chooser.Open(side_box))  goto close; 
	promotion_chooser.Set_Visible(false);

	top_box.Set_Next(true);
	if (! board.Open
	    (top_box,
	     Board::Open_Info(Internet::Current_Position(),
			      "move",
			      "", 
			      "",  "", "",
			      Internet::Current_Generator(),
			      NULL,
			      false, 
			      menubar)))
		goto close;
	board.notation= &notation_preferences();
	board.notation_settings= &notation_settings_preferences(); 
	

	if (! statusbar.Open(overall_box, 2))  goto close;

	if (Settings_Simple_Load(internet_account, "internet_account"))
	{
		Login();
	}
	else
	{
		internet_account.Reset();
		Update_Connected();
		account_editor.Open("account_edited", *this);
	}

	Verbose_Int(side_box.Ref_Count()); 
	Verbose_Pointer((Pointed *)&side_box); 

	return true;

 close:
	Window::Close(); 
	return false;
}

void Internet_Window::Wdg_Close()
{
	Verbose_Function(); 

	Verbose_Int(side_box.Ref_Count()); 

	Logout(true);
	if (interaction_window.Is())
		interaction_window.Close(true); 
	notation_preferences.Close(); 
	notation_settings_preferences.Close();
	account_editor.Close();

	Verbose_Int(side_box.Ref_Count()); 

	Window::Wdg_Close();
}

void Internet_Window::MR_Receive(String message)
{
	if (!memcmp(message(), "board_", 6))
		board.MR_Receive(message);
	
	else if (message == "notation_changed" ||
		 message == "notation_settings_changed")
	{
		Update_Position(); 
	}

	else if (message == "input")
	{
		if (Internet::Proceed())
		{
			Logout();
			if (Internet::Error().Is())
			{
				Error::Set(Internet::Error());
				Inform_Error(_("Logged out"), *this);
			}
		}
	}

	/* User move */ 
	else if (message == "move")
	{
		int from, to;
		if (! board.Get_Squares(from, to))  return;

		if (Internet::Is() && Internet::Current_Generator().Is())
		{
			int index= Internet::Current_Generator().Square_Move(from, to);
			if (index < 0)  return;
			index += promotion_chooser.Selection();
			User_Play(index, from, to);
		}

		if (to)  board.Invalidate_Square(to, false);
		board.Invalidate_Square(from, false);
		promotion_chooser.Update(Internet::Current_Position());
	}
	else if (message == "account_edited")
	{
		internet_account= account_editor.Value();
		Settings_Simple_Save(internet_account, "internet_chess_account");
		Login();
		account_editor.Have_Read();
	}
	else if (message == "login")
	{
		account_editor.Open("account_edited", 
				    *this,
				    internet_account,
				    this);
	}
	else if (message == "logout")
	{
		Logout();
	}
	else if (message == "seek")
	{
		if (Internet::Is() && ! Internet::Inside_Game())
			Seek(5, 5);
	}
	else if (message == "resign")
	{
		if (Internet::Is() && Internet::Inside_Game())
			Internet::Resign();
	}
	else if (message == "text")
	{
		if (! Internet::Is())  return; 
		if (! interaction_window.Is())  return;
		Internet::Put(interaction_window.Pull_Text());
	}
	else if (message == "about")
		New_About();
	else if (message == "interaction")
	{
		if (! interaction_window.Is())
		{
			Open_Interaction_Window();
		}
		else 
		{
			interaction_window.Close(); 
		}
	}
	else if (message == "legal_move_list")
	{
		if (! legal_move_list.Is())  return;
		int move= legal_move_list->Selection();
		if (move < 0)  return;
		User_Play(move); 
	}
	else if (message == "show_legal")
	{
		if (legal_move_list.Is())
		{
			legal_move_list.Close();
		}
		else
		{
			side_box.Set_Next(true);
			if (! legal_move_list.Open(side_box, "legal_move_list"))
				return;
			Update_Legal_Moves();
		}
	}
	else if (message == "notation")
		notation_preferences.Edit(); 
	else if (message == "edit_notation_settings")
		notation_settings_preferences.Edit();
	else if (message == "interaction_close")
	{
		menubar.Set_Active("/View/Messages", false); 
		Object_Reset(Internet_Interaction_Window, &interaction_window);
	}
	else 
		Window::MR_Receive(message);
}

void Internet_Window::Seek(int m, int s)
{
	assert (! Internet::Inside_Game());
	Internet::Seek(m, s);
	statusbar.Set(0, String(_("Seeking")) + "...");
}

void Internet_Window::Login()
{
	Verbose_Function();

	if (Internet::Is())
		Internet::Close();

	if (! Internet::Open(internet_account.host,
			     internet_account.port,
			     internet_account.user,
			     internet_account.password))
	{
		goto error;
	}

	{

		String user= Internet::User();
		String host= Internet::Host();

		if (Input_Reader::Is())
			Input_Reader::Close();
		Input_Reader::Open(fileno(Internet::Output()));

		if (Internet::Proceed())
		{
			Logout();
			Error::Set(Internet::Error());
			goto error;
		}

		Verbose("\tsuccess");

		Update_Connected(host, user);
	}
	return;
	
 error:
	Verbose("\terror"); 
	Inform_Error(Format(_("Cannot connect to %s"), internet_account.host()),
		     *this);
	return; 
}

void Internet_Window::Logout(bool omit_updates)
{
	if (! Internet::Is())   return; 

	Input_Reader::Close();
	Internet::Close();
	
	if (omit_updates)  return; 

	Update_Connected(); 

	promotion_chooser.Set_Visible(false);
	material_widget.Hide();
}

void Internet_Window::Internet_Chess_Changed()
{
	board.Invalidate_Position();
	statusbar.Set
		(0,
		 _(Internet::Current_Position().To_Move() == cc_w
		   ? N_("White to move")
		   : N_("Black to move")));
}

void Internet_Window::Internet_Chess_New_Game()
{
	bool turned= board.Turned();
	bool new_turned= Internet::Playing_Color();
	if (turned != new_turned)
	{
		board.Set_Turned(new_turned);
		players_display.Set_Turned(new_turned);
	}
	players_display.Set_Name(cc_w, Internet::White());
	players_display.Set_Name(cc_b, Internet::Black());
	
	statusbar.Set(1, _("New game"));
	players_display.Set_To_Move(Internet::Current_Position().To_Move());
	board.Clear_Last_Move();
	Update_Position();
}

void Internet_Window::Internet_Chess_Finished()
{
	int r= Internet::Result();
	statusbar.Set
		(0,
		 _(r == 0 ? N_("Black wins") :
		   r == 1 ? N_("Draw")       :
		   r == 2 ? N_("White wins") :
		   N_("Aborted")));
	players_display.Set_To_Move(-1); 
	Update_Position();
	promotion_chooser.Set_Visible(false);
	material_widget.Hide();
}

void Internet_Window::Internet_Chess_Playing(String move)
{
	Position p;
	p= Internet::Current_Position();
	Generator generator;
	generator.New(p);
	int sm= Parse_Move(move(), generator);
	if (sm >= 0)
	{
		Move m= generator.Get_Move(sm);
		board.Set_Last_Move(m.from,
				    m.to);
		move=
		Format_Move_Name(generator.Get_Move_Name(sm),
				 notation_preferences(), 
				 notation_settings_preferences());
	}
	else
		board.Clear_Last_Move();
	statusbar.Set(1, move);
	Update_Position();
	promotion_chooser.Update(Current_Position());
}

void Internet_Window::Internet_Chess_Message(String message)
{
	if (interaction_window.Is())
		interaction_window.Append_Message(message);
}

void Internet_Window::Internet_Chess_Clock(int white, int black)
{
	players_display.Set_Clock(cc_w, white);
	players_display.Set_Clock(cc_b, black);
}

void Internet_Window::IR_Input()
{
	MR_Receive("input");
}


void Internet_Window::Update_Position()
{
	if (legal_move_list.Is())
	{
		Update_Legal_Moves();
	}

	material_widget.Update(Current_Position(),
			       Playing_Color(),
			       side_box);
}

void Internet_Window::Open_Interaction_Window()
{
	interaction_window.Open(NULL, this, "text", "interaction_close");
	gtk_widget_show(interaction_window.Root().Get());
}

void Internet_Window::User_Play(int move_index,
				int from,
				int to)
{
	if (from == -1)
		from= Internet::Current_Generator()[move_index].from;
	if (to == -1)
		to= Internet::Current_Generator()[move_index].to;

	if (Internet::Is() && Internet::User_Play(from, to))
	{
		statusbar.Set
			(1,
			 Format_Move_Name
			 (Internet::Current_Generator().Get_Move_Name(move_index),
			  notation_preferences(),
			  notation_settings_preferences()));
		board.Set_Last_Move(from, to);
		Update_Position();
		promotion_chooser.Update(Current_Position());
	}
}

void Internet_Window::Update_Legal_Moves()
{
	if (! Internet::Current_Generator().Is())
		return; 
	legal_move_list.Set(Internet::Current_Generator(),
			    notation_preferences(),
			    notation_settings_preferences());
}

void Internet_Window::Update_Connected(String host, String user)
{
	bool connected= host;

	/* Status bar */ 
	statusbar.Set(0, 
		       connected 
		       ? Format(_("Connected to %s"), host()) 
		       : _("Not connected")); 

	/* Window title */ 
	const char *title= _("Internet Chess"); 
	String text;
	if (connected)
	{
		String text= Format("%s@%s – %s", 
				    user(), host(),
				    title); 
	}
	else
		text= title; 
	gtk_window_set_title(GTK_WINDOW(Root().Get()), text());

	/* Enable/disable menu entries/toolbar buttons */
	menubar.Set_Sensitive("/Connection/Login", !connected);
	menubar.Set_Sensitive("/Connection/Logout", connected);
	gtk_widget_set_sensitive(login_button, !connected);
	gtk_widget_set_sensitive(logout_button, connected);
}
