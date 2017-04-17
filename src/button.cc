#include "button.hh"

#include <gtk/gtksignal.h>

void Button::Signal_Clicked(GtkButton *,
			    gpointer *data)
{
	Button *button= (Button *)data;
	button->MR_Receive(button->msg_clicked);
}

bool Button::Open(Pointer <Widget> parent,
		  String new_msg_clicked)
{
	msg_clicked= new_msg_clicked;
	if (! Widget::Open(parent, gtk_button_new()))
		return false;

	gtk_signal_connect(GTK_OBJECT(Get()), 
			   "clicked",
			   GTK_SIGNAL_FUNC(Signal_Clicked), 
			   this);
	return true; 
}
