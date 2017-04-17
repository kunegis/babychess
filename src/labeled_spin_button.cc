#include "labeled_spin_button.hh"

#include "gettext.hh"
#include "format.hh"

#include <gtk/gtklabel.h>

bool Labeled_Spin_Button::Open(Pointer <Widget> parent,
			       const char *label_text,
			       Spin_Button::Open_Info open_info)
{
	if (! H_Box::Open(parent, dialog_unit))
		return false;
	if (! label.Open(this, Format("%s%s", label_text, _(":")), 0.0, "", true))
		goto close;
	Set_Next(true); 
	if (! button.Open(this, open_info))
		goto close;
	gtk_label_set_mnemonic_widget(GTK_LABEL(label.Get()), button.Get()); 
	return true;
 close:
	Close(); 
	return false;
}
