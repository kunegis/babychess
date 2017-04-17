#ifndef MENU_HH
#define MENU_HH

#include "widget.hh"
//#include "menu_callback.hh"

#include <glib/gtypes.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkitemfactory.h>
#include <gtk/gtkmenu.h>

void Menu_Callback( gpointer    callback_data,
		    guint       callback_action,
		    GtkWidget  *widget );

#define MENU_CALLBACK  ((void (*)())&Menu_Callback)

extern bool menu_callback_active;

class Menubar
	:  public Widget
{
public:
	bool Open(Pointer <Widget> parent,
		  int count,
		  GtkItemFactoryEntry menu_items[],
		  Widget &receiver);
	void Append(int count,
		    GtkItemFactoryEntry menu_items[],
		    Widget &receiver);

	void Set_Sensitive(const char *item, bool sensitive);
	bool Active(const char *);
	void Set_Active(const char *name, bool active);
	void Remove_Item(const char *name);

private:
	GtkItemFactory *item_factory; 

};



class Widget;


class Popup_Menu
{
public:

	Popup_Menu(int count,
		   GtkItemFactoryEntry menu_items[],
		   Widget &receiver);
//		   const char *(*translate_func)(const char*)= NULL);

	~Popup_Menu();

	void Popup();
	void Set_Active(const char *name, bool active);

private:

	/* Both variables are set in the constructor. 
	 */
	GtkItemFactory *item_factory; 
	GtkMenu *menu;

	void Close();
	/* called only from destructor */ 
};


#endif /* ! MENU_HH */
