#include "spin_button.hh"

#include <gtk/gtksignal.h>

bool Spin_Button::Open(Pointer <Widget> parent,
		       Open_Info open_info)
{
	adjustment= GTK_ADJUSTMENT
		(gtk_adjustment_new
		 (open_info.value, 
		  open_info.min,
		  open_info.max, 
		  1, 100, 1000));

	if (adjustment == NULL)
		return false;

	if (! Widget::Open
	    (parent,
	     gtk_spin_button_new(adjustment, 1, 0)))
	{
		gtk_object_destroy(GTK_OBJECT(adjustment));
		return false;
	}
	msg_changed= open_info.msg_changed;

	in= true;
	gtk_signal_connect(GTK_OBJECT(Get()), 
			   "changed",
			   GTK_SIGNAL_FUNC(Signal_Changed), 
			   this);
	in= false;
	return true;
}

void Spin_Button::Signal_Changed(GtkEditable *editable,
			      gpointer data)
{
	Verbose_Function();

	Spin_Button *spin_button= (Spin_Button *)data;
	assert (spin_button != NULL);
	assert (spin_button->Get() == GTK_WIDGET(editable));
	if (spin_button->in)  return; 

	spin_button->MR_Receive(spin_button->msg_changed);
}

void Spin_Button::Set(int value)
{
	in= true;
	String save_msg_changed= msg_changed;
	gtk_spin_button_set_value
		(GTK_SPIN_BUTTON(Get()),
		 value); 
	msg_changed= save_msg_changed;
	in= false;
}

void Spin_Button::Wdg_Close()
{
	gtk_object_destroy(GTK_OBJECT(adjustment));
	Widget::Wdg_Close();
}
