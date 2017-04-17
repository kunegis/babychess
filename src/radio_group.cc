#include "radio_group.hh"

#include <stdio.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtksignal.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtktable.h>

bool Text_Radio_Button::Open(Pointer <Widget> parent,
			     String text,
			     String new_msg_changed,
			     Pointer <Widget> first)
{
	GtkWidget *w;

	if (!first)
		w= gtk_radio_button_new_with_mnemonic(NULL, text());
	else
		w= gtk_radio_button_new_with_mnemonic_from_widget
			(GTK_RADIO_BUTTON(first().Get()),
			 text());

	if (w == NULL)  return false;

	if (! Widget::Open(parent, w))  return false;

	gtk_signal_connect(GTK_OBJECT(Get()), 
			   "clicked",
			   GTK_SIGNAL_FUNC(Signal_Clicked), 
			   this);

	msg_changed= new_msg_changed; 

	return true;
}

void Text_Radio_Button::Signal_Clicked(GtkButton *gtkbutton,
				       gpointer *data)
{
	Verbose_Function(); 
	Text_Radio_Button *button= (Text_Radio_Button *)data;
	assert (button != NULL);
	assert (button->Get() == GTK_WIDGET(gtkbutton));
	Verbose_Str(button->msg_changed());
	if (button->msg_changed)
		button->Parent_Receive_Message(button->msg_changed); 
}

void Text_Radio_Button::Set_Checked(bool checked)
{
	/* gtk_toggle...() sends the signal.  */
	
	String save_msg_changed= msg_changed;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Get()),
				     checked ? TRUE : FALSE); 
	msg_changed= save_msg_changed;
}

void Text_Radio_Button::Set_Text(String text)
{
	gtk_button_set_label(GTK_BUTTON(Get()), text()); 
}


Radio_Group &Radio_Group::operator << (String text)
{
	assert (Is());

	if (! buttons.Set(*buttons + 1))
		return *this;

	Dynamic <Text_Radio_Button> text_radio_button;
	if (! text_radio_button.Alloc())
	{
		buttons= -1;
		return *this;
	}

	if (! text_radio_button().Open
	    (this, 
	     text,
	     Format("clicked %d", *buttons-1),
	     *buttons == 1 ? NULL : & buttons[0]()))
	{
		buttons= -1;
		return *this;
	}
	
	text_radio_button.Transfer_To(buttons[-1]);

	return *this; 
}

void Radio_Group::MR_Receive(String message)
{
	if (! memcmp(message(), "clicked ", 8))
	{
		int index;
		if (1 != sscanf(message()+8, "%d", &index))  return;
		if (index < 0 || index >= *buttons)  return;
		if (selection == index)  return;
		selection= index;
		Parent_Receive_Message(msg_changed);
	}
	else
		return Table::MR_Receive(message); 
}

bool Radio_Group::Open(Pointer <Widget> parent, 
				 Open_Info open_info)
{
	assert (open_info.columns >= 1);

	msg_changed= open_info.msg_changed;
	selection= 0; 
	columns= open_info.columns;
	buttons.Free();

	if (! Table::Open(parent, columns))  return false;
	gtk_table_set_homogeneous(GTK_TABLE(Get()), FALSE);
	
	return true;
}

void Radio_Group::Set_Selection(int new_selection)
{
	assert (new_selection >= -1 && new_selection < *buttons);
	
	if (selection == new_selection)
		return;
	String save_msg_changed= msg_changed;
	msg_changed= "";
	if (selection != -1)
		buttons[selection]().Set_Checked(false);
	selection= new_selection;
	if (selection != -1)
		buttons[selection]().Set_Checked(true);
	msg_changed= save_msg_changed; 
}

void Radio_Group::Set_Text(int i, String text)
{
	buttons[i]().Set_Text(text); 
}

