#ifndef OPTION_MENU_HH
#define OPTION_MENU_HH

#include "widget.hh"

#include <gtk/gtkmenu.h>
#include <gtk/gtkoptionmenu.h>

class Option_Menu
	:  public Widget
{
public:
	Option_Menu()
		:  menu(NULL)   {  }

	bool Open(Pointer <Widget> parent,
		  String new_msg_changed= "");
	Option_Menu &operator << (String text);

	void Update();
	/* must be called after having set up the content with
	 * operator << (). */

	void Set_Selection(int index);
	int Selection() const;
	int Count() const;
	void Clear();

private:

	GtkMenu *menu;
	String msg_changed; 
	
	static bool in;

	void Wdg_Close();

	static void Signal_Changed (void *, Option_Menu *b);
};

#endif /* ! OPTION_MENU_HH */ 
