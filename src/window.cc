#include "window.hh"

#include "icon.hh"

#include <gtk/gtkwindow.h>

bool Window::Open(String caption,
		  GdkPoint size)
{
	GtkWidget *gtkwidget= gtk_window_new(GTK_WINDOW_TOPLEVEL);
	if (gtkwidget == NULL)  return false;
	gtk_window_set_title(GTK_WINDOW(gtkwidget),
			     caption()); 

	Set_Icon(GTK_WINDOW(gtkwidget));

	if (size.x != -1)
		gtk_window_set_default_size(GTK_WINDOW(gtkwidget),
					    size.x, size.y);
	else
		gtk_window_set_policy(GTK_WINDOW(gtkwidget), FALSE, FALSE, TRUE); 

	return Widget::Open(NULL, gtkwidget, true);
}
