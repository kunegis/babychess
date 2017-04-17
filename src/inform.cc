#include "inform.hh"

#include "gettext.hh"
#include "error.hh"
#include "format.hh"

GtkWidget *inform_last;

void Inform(const char *text,     
	    const char *secondary_text,
	    GtkMessageType type,
	    Widget *transient_for)
{
	Verbose_Function(); 

	inform_last= gtk_message_dialog_new
		(transient_for ? GTK_WINDOW(transient_for->Root().Get()) : NULL,
		 GTK_DIALOG_DESTROY_WITH_PARENT,
		 type,
		 GTK_BUTTONS_OK,
		 text);

	if (inform_last == NULL)  return;

	if (secondary_text)
		gtk_message_dialog_format_secondary_text
			(GTK_MESSAGE_DIALOG(inform_last),
			 secondary_text);

	if (transient_for)
		gtk_window_set_transient_for
			(GTK_WINDOW(inform_last), 
			 GTK_WINDOW(transient_for->Root().Get()));
	gtk_widget_show(inform_last);
}

void Inform_Error(String what,
		  Pointer <Widget> widget)
{
	Inform(what(),
	       Error::Get()(),
	       GTK_MESSAGE_ERROR,
	       widget);
}
