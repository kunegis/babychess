#include "piece_radio_button.hh"

#include <gtk/gtkradiobutton.h>
#include <gtk/gtksignal.h>
#include <gtk/gtktogglebutton.h>

bool Piece_Radio_Button::Open(Pointer <Widget> parent,
			      int p,
			      String new_msg_changed,
			      Pointer <Widget> first)
{
	GtkWidget *w;
	if (first)
		w= gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(first().Get()));
	else 
		w= gtk_radio_button_new(NULL);

	if (! w)  return false;

	if (! Widget::Open(parent, w))   return false; 
	
	if (! piece.Open(this, p, dialog_unit*5 / 2))  goto close;
	
	gtk_signal_connect(GTK_OBJECT(Get()), 
			   "clicked",
			   GTK_SIGNAL_FUNC(Signal_Clicked), 
			   this);

	msg_changed= new_msg_changed; 
	
	return true;
	
 close:
	Close(); 
	return false;
}

void Piece_Radio_Button::Signal_Clicked(GtkButton *gtkbutton,
				       gpointer *data)
{
	Piece_Radio_Button *button= (Piece_Radio_Button *)data;
	assert (button != NULL);
	assert (button->Get() == GTK_WIDGET(gtkbutton));

	button->Parent_Receive_Message(button->msg_changed); 
}

void Piece_Radio_Button::Set_Checked(bool checked)
{
	String save_msg_changed= msg_changed;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Get()),
				     checked ? TRUE : FALSE); 
	msg_changed= save_msg_changed;
}
