#include "text_button.hh"

#include <gtk/gtksignal.h>

void Text_Button::Signal_Clicked(GtkButton *,
				 gpointer *data)
{
	Text_Button *text_button= (Text_Button *)data;
	text_button->MR_Receive(text_button->msg_clicked);
}

bool Text_Button::Open(Pointer <Widget> parent,
		       Open_Info open_info)
{
	msg_clicked= open_info.msg_clicked;
	if (! Widget::Open
	    (parent,
	     open_info.stock 
	     ? gtk_button_new_from_stock(open_info.stock)
	     : gtk_button_new_with_mnemonic(open_info.text())))
		return false;
	gtk_widget_set_sensitive(Get(), open_info.sensitive);
	gtk_signal_connect(GTK_OBJECT(Get()), 
			   "clicked",
			   GTK_SIGNAL_FUNC(Signal_Clicked), 
			   this);
			
	return true; 
}
