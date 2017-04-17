#include "scrolled.hh"

#include <gtk/gtkscrolledwindow.h>

bool Scroll_Widget::Open(Pointer <Widget> parent,
			 Open_Info oi)
{
	if (! Widget::Open(parent, gtk_scrolled_window_new(NULL, NULL)))
		return false;
	gtk_scrolled_window_set_policy
		(GTK_SCROLLED_WINDOW(Get()),
		 oi.scroll_direction & scr_horizontal
		 ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER,
		 oi.scroll_direction & scr_vertical
		 ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER);
	return true; 
}
