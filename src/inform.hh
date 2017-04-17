#ifndef INFORM_HH
#define INFORM_HH

/* Display a message window  */

#include "string.hh"
#include "widget.hh"

#include <gtk/gtkmessagedialog.h>

extern GtkWidget *inform_last;

void Inform(const char *text,     
	    const char *secondary_text,
	    GtkMessageType type,
	    Widget *transient_for= NULL);

void Inform_Error(String what,
		  Pointer <Widget> transient_for);

#endif /* ! INFORM_HH */
