#include "check_box.hh"

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtksignal.h>

bool Check_Box::Open(Pointer <Widget> parent,
		     bool checked,
		     String text,
		     String new_msg_clicked)
{
	if (! Widget::Open(parent, gtk_check_button_new_with_mnemonic(text())))
		return false;

	msg_clicked= new_msg_clicked; 

	gtk_signal_connect(GTK_OBJECT(Get()), 
			   "clicked",
			   GTK_SIGNAL_FUNC(Signal_Clicked), 
			   this);
	
	Set_Checked(checked); 

	return true; 
}

void Check_Box::Signal_Clicked(GtkButton *gtkbutton,
			       gpointer *data)
{
	Check_Box *check_box= (Check_Box *)data;
	assert (check_box != NULL);
	assert (check_box->Get() == GTK_WIDGET(gtkbutton));

	check_box->MR_Receive(check_box->msg_clicked);
}

void Check_Box::Set_Checked(bool checked)
{
	String omsg= msg_clicked;
	msg_clicked= ""; 
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Get()),
				     checked ? TRUE :
				     FALSE); 
	msg_clicked= omsg;
}

bool Check_Box::Checked() const
{
	return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(Get()));  
}
