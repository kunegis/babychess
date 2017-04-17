#ifndef OPENING_WIDGET_HH
#define OPENING_WIDGET_HH

#include "book.hh"
#include "label.hh"
#include "opening_book.hh"
#include "box.hh"
#include "opening_settings.hh"
#include "opening_settings_editor.hh"
#include "opening_settings_stream.hh"
#include "preferences.hh"
#include "menu.hh"

#include "../dep/strings.hh"

class Opening_Widget
	:  public V_Box,
	   public Opening_Book
{
public:
	void MR_Receive(String);

	bool Open(Pointer <Widget> parent,
		  Menubar &, 
		  const Game &,
		  const char *msg_options_changed);
	void Update(); 
	String Name(move_t small_move)
		{
			if (! (Opening_Book::Is() || Opening_Book::Open()))
				return "";
			return Opening_Book::Name(small_move, preferences().flags);
		}

private:
	Preferences <Opening_Settings_Editor> preferences; 
	Pointer <Menubar> menubar; 
	Pointer <const Game>    game; 
	const char *msg_options_changed; 

	Label label; 

	void Wdg_Close();
	
	void Update_Menu()
		{
			Set_Menu_Subvariation();
			Set_Menu_Alternative();
		}
	void Set_Menu_Subvariation()
		{
			menubar->Set_Active("/Openings/Show subvariation names",
					    preferences->flags & op_subvariation);
		}
	void Set_Menu_Alternative()
		{
			menubar->Set_Active("/Openings/Show alternative names",
					     preferences->flags & op_alternative);
		}
};

#define OPENING_WIDGET_MENU       \
   {"/"N_("_Openings"),                             NULL,     \
    NULL,         0,                         "<Branch>", 0},\
   {"/Openings/"N_("Show _subvariation names"),        NULL,     \
    MENU_CALLBACK,str_opening_subvariation,"<CheckItem>",  0},  \
   {"/Openings/"N_("Show _alternative names"),        NULL,     \
    MENU_CALLBACK,str_opening_alternative,"<CheckItem>",  0},  \
   { "/Openings/sep0",                                      NULL, \
     NULL,         0,                    "<Separator>", 0},\
   {"/Openings/"N_("_Preferences"),       NULL,         \
    MENU_CALLBACK,  str_opening_edit,   "<StockItem>", GTK_STOCK_PREFERENCES},

#endif /* ! OPENING_WIDGET_HH */ 
