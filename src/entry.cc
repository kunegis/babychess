#include "entry.hh"

bool Entry::Open(Pointer <Widget> parent,
		Open_Info open_info)
{
	if (! Widget::Open(parent,
			   gtk_entry_new()))
		return false;
	gtk_entry_set_max_length(GTK_ENTRY(Get()), open_info.length);
	gtk_entry_set_activates_default(GTK_ENTRY(Get()), TRUE);
	if (open_info.msg || open_info.msg_changed)
	{
		Verbose("entry:  connect");
		Connect_Key_Signals(Get());
	}
	Set(open_info.text);
	if (open_info.hidden)
		gtk_entry_set_visibility(GTK_ENTRY(Get()), FALSE); 
	msg= open_info.msg;
	msg_changed= open_info.msg_changed;
	return true;
}

bool Entry::Wdg_Key_Press(guint keyval)
{
	Verbose("entry::key press(%u)", keyval); 
	/* If we do a receive here, the message will arrive before the
	 * key press has been processed internally by GTK+, and doing
	 * get_text() will retrieve the old text. 
	 */
	Parent_Send_Message(msg_changed); 
	if (keyval == GDK_Return)
	{
		Parent_Receive_Message(msg);
		return true;
	}
	else
		return false;
}

#include "gettext.hh"

#include <gtk/gtklabel.h>

bool Labeled_Entry::Open(Pointer <Widget> parent,
			 const char *label_text)
{
	if (! H_Box::Open(parent, dialog_unit))
		return false;
	if (! label.Open(this, String(label_text).operator+(_(":")), 0.0, "", true))
		goto close;
	H_Box::Set_Next(true); 
	if (! entry.Open(this))  goto close;
	gtk_label_set_mnemonic_widget(GTK_LABEL(label.Get()), entry.Get()); 
	return true;
 close:
	Close(); 
	return false;
}
