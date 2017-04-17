#include "bevel.hh"

#include <gtk/gtkhseparator.h>
#include <gtk/gtkvseparator.h>

bool Bevel::Open(Pointer <Widget> parent,
		 bool direction_h)
{
	return Widget::Open(parent,
			    direction_h
			    ? gtk_hseparator_new()
			    : gtk_vseparator_new());
}
