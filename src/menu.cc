#include "menu.hh"

#include "widget.hh"
#include "gettext.hh"

#include <libintl.h>
#include <gtk/gtkmenubar.h>
#include <gtk/gtkcheckmenuitem.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkcheckmenuitem.h>

#define STRINGS_DEFINE
#  include "../dep/strings.hh"
#undef STRINGS_DEFINE

bool menu_callback_active= true;

void Menu_Callback(gpointer    callback_data,
		   guint       callback_action,
		   GtkWidget  *)
{
	if (! menu_callback_active)  return;

	Widget &widget= *(Widget *)callback_data;

	widget.MR_Receive(string_list[callback_action]);
}



static const char *Menu_Translate(const char *s)
{
	Verbose_Str(s);
	const char *t= strrchr(s, '/');
	if (t == NULL)
		t= s;
	else
		++t;
	Verbose_Str(t);
	const char *ret= gettext(t); 
	Verbose_Str(ret);
	return ret;
}


bool Menubar::Open(Pointer <Widget> parent,
		    int count,
		    GtkItemFactoryEntry menu_items[],
		    Widget &receiver)
{
	GtkAccelGroup *accel_group= gtk_accel_group_new();

	item_factory= gtk_item_factory_new
		(GTK_TYPE_MENU_BAR, "<main>",
		 accel_group);

	gtk_item_factory_set_translate_func
		(item_factory, (GtkTranslateFunc)&Menu_Translate, 0, NULL); 

	gtk_item_factory_create_items(item_factory, count, menu_items, &receiver);

	gtk_window_add_accel_group (GTK_WINDOW(parent->Root().Get()), accel_group);

	return Widget::Open
		(parent, 
		 gtk_item_factory_get_widget  
		 (item_factory, "<main>"));
}

void Menubar::Set_Sensitive(const char *item, bool sensitive)
{
	GtkWidget *i= gtk_item_factory_get_item(item_factory, item);
	if (i == NULL)
	{
		fprintf(stderr, "name= %s\n", item);
		assert (i);
	}
	gtk_widget_set_sensitive (i, sensitive); 
}

void Menubar::Remove_Item(const char *name)
{
	GtkWidget *i= gtk_item_factory_get_item(item_factory, name);
	assert (i != NULL);
	gtk_widget_destroy(i); 
}

void Menubar::Append(int count,
		      GtkItemFactoryEntry menu_items[],
		      Widget &receiver)
{
	gtk_item_factory_create_items(item_factory, count, menu_items, &receiver);
}

bool Menubar::Active(const char *name)
{
	return gtk_check_menu_item_get_active
		(GTK_CHECK_MENU_ITEM
		 (gtk_item_factory_get_item
		  (item_factory, name)));
}

void Menubar::Set_Active(const char *name, 
			 bool active)
{
	menu_callback_active= false;
	GtkWidget *item= gtk_item_factory_get_item(item_factory, name);
#ifndef NDEBUG
	if (item == NULL)
	{
		fprintf(stderr, "name= %s\n", name);
		assert (item);
	}
#endif /* ! NDEBUG */ 
	gtk_check_menu_item_set_active
		(GTK_CHECK_MENU_ITEM(item), active);
	menu_callback_active= true;
}


void Popup_Menu::Popup()
{
	gtk_menu_popup(menu,
		       NULL, NULL, NULL, NULL,
		       0, 0);
}

void Popup_Menu::Close()
{
	if (item_factory == NULL)
		return; 

	if (menu != NULL)
	{
		gtk_object_destroy(GTK_OBJECT(menu));
	}

	gtk_object_destroy(GTK_OBJECT(item_factory));
}

Popup_Menu::Popup_Menu(int count,
			   GtkItemFactoryEntry menu_items[],
		       Widget &receiver)
//			   const char *(*translate_func)(const char*))
{
	item_factory= gtk_item_factory_new(GTK_TYPE_MENU, "<main>", NULL);

	if (item_factory == NULL)
	{
		menu= NULL;
		return;
	}

//	if (!translate_func)  translate_func= (const char *(*)(const char *))gettext;
	gtk_item_factory_set_translate_func
		(item_factory, (GtkTranslateFunc)Menu_Translate, 0, NULL); 
	

	gtk_item_factory_create_items(item_factory, count, menu_items, &receiver);
	
	menu= GTK_MENU(gtk_item_factory_get_widget(item_factory, "<main>")); 
}

Popup_Menu::~Popup_Menu()
{
	Close();
}

void Popup_Menu::Set_Active(const char *name, 
			      bool active)
{
	menu_callback_active= false;
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(item_factory, name)), active);
	menu_callback_active= true;
}

