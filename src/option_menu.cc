#include "option_menu.hh"

#include <gtk/gtksignal.h>
#include <gtk/gtkmenuitem.h>

bool Option_Menu::in= false;

bool Option_Menu::Open(Pointer <Widget> parent,
		       String new_msg_changed)
{
	msg_changed= new_msg_changed;
	if (! Widget::Open(parent, 
			   gtk_option_menu_new()))
		return false;
	menu= GTK_MENU(gtk_menu_new()); 
	if (menu == NULL)
	{
		Close();
		return false;
	}
	gtk_signal_connect(GTK_OBJECT(Get()),
			   "changed",
			   GTK_SIGNAL_FUNC(& Signal_Changed),
			   this);
	return true;
}

Option_Menu & Option_Menu::operator << (String text)
{
	assert(Is()); 
	assert (menu != NULL);
	GtkWidget *item= gtk_menu_item_new_with_mnemonic(text());
	gtk_widget_show(item);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	return *this; 
}

void Option_Menu::Signal_Changed (void *,
				  Option_Menu *b)
{
	Verbose_Function();
	Option_Menu &box= *(Option_Menu *)b;
	if (in)  return;
	box.MR_Receive(box.msg_changed);
}

void Option_Menu::Clear()
{
	gtk_option_menu_remove_menu(GTK_OPTION_MENU(Get()));
	/* MENU is destroyed by the remove_menu(). 
	 */
	menu= GTK_MENU(gtk_menu_new());
	Update(); 
}

void Option_Menu::Wdg_Close()
{
	gtk_object_destroy(GTK_OBJECT(menu));
	menu= NULL; 
	Widget::Wdg_Close();
}

void Option_Menu::Update()
{
	in= true;
	gtk_option_menu_set_menu(GTK_OPTION_MENU(Get()), GTK_WIDGET(menu));
	in= false;
}

void Option_Menu::Set_Selection(int index)
{
	in= true;
	gtk_option_menu_set_history(GTK_OPTION_MENU(Get()),
				    index);
	in= false; 
}

int Option_Menu::Selection() const
{ 
	return gtk_option_menu_get_history(GTK_OPTION_MENU(Get()));  
}

int Option_Menu::Count() const
{
	GList *children= gtk_container_get_children(GTK_CONTAINER(menu)); 
	int ret= g_list_length(children);
	g_list_free(children);
	return ret; 
}
