#include "opening_widget.hh"

#include "gettext.hh"
#include "format.hh"

#include <gtk/gtklabel.h>

bool Opening_Widget::Open(Pointer <Widget> parent,
			  Menubar &new_menubar,
			  const Game &new_game,
			  const char *new_msg_options_changed)
{
	if (! V_Box::Open(parent))
		return false;

	if (! label.Open(this, "", 0.0, "<"))  return false;
	
	gtk_label_set_line_wrap(GTK_LABEL(label.Get()), TRUE);

	preferences.Open(*this, "edited");

	Opening_Book::Open();

	menubar= new_menubar; 
	game= new_game; 
	msg_options_changed= new_msg_options_changed; 

	Update();
	Update_Menu();

	return true;
}

void Opening_Widget::Update()
{
	String eco;
	Vector <String> names;
	if (Opening_Book::Is() || Opening_Book::Open())
		Opening_Book::Get(game(), names, preferences->flags, eco);

	if (*names)
	{
		gtk_widget_show(label.Get());
		String text;
		text += Format ("<span weight = \"bold\">%s</span>",
				names[0]());
		for (int i= 1; i < *names; ++i)
		{
			text += ", ";
			text += names[i];
		}
		if (eco)
			text += Format(" (%s)", eco());
		label.Set(text); 
	}
	else
		gtk_widget_hide(label.Get()); 
}

void Opening_Widget::Wdg_Close()
{
	preferences.Close();
	menubar= NULL; 
	game= NULL; 
	V_Box::Wdg_Close(); 
}

void Opening_Widget::MR_Receive(String message)
{
	if (message == "edit")
	{
		preferences.Edit();
	}
	else if (message == "edited")
	{
		Update();
		Update_Menu();
		Parent_Receive_Message(msg_options_changed); 
	}
	else if (message == "subvariation")
	{
		preferences.Get_Change_Value().flags ^= op_subvariation;
		preferences.Changed();
		Set_Menu_Subvariation();
		Update();
		Parent_Receive_Message(msg_options_changed); 
	}
	else if (message == "alternative")
	{
		preferences.Get_Change_Value().flags ^= op_alternative;
		preferences.Changed();
		Set_Menu_Alternative();
		Update(); 
		Parent_Receive_Message(msg_options_changed); 
	}
	else
		V_Box::MR_Receive(message); 
}
