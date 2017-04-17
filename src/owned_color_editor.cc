#include "owned_color_editor.hh"

#include "gettext.hh"

#include <gtk/gtkcolorseldialog.h>
#include <gtk/gtksignal.h>

void Owned_Color_Editor::Open(String new_msg,
			      Widget &new_owner, 
			      const RGB &rgb)
{
	if (widget != NULL)
		return;

	msg= new_msg;
	owner= new_owner;

	widget= gtk_color_selection_dialog_new(_("Choose a Color"));
	
	if (widget == NULL)
		return;

	gtk_window_set_transient_for(GTK_WINDOW(widget),
				     GTK_WINDOW(new_owner.Root().Get()));
	
	gdouble d[4];
	d[0]= ((double)rgb.r) / 256.0;
	d[1]= ((double)rgb.g) / 256.0;
	d[2]= ((double)rgb.b) / 256.0;
	d[3]= 0;
	gtk_color_selection_set_color
		(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(widget)->colorsel),
		 d);
	gtk_signal_connect (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG(widget)->ok_button),
			    "clicked", GTK_SIGNAL_FUNC(& Signal_OK),
			    this);

	gtk_signal_connect
		(GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG(widget)->cancel_button),
		 "clicked", GTK_SIGNAL_FUNC(& Signal_Cancel),
		 this);

	gtk_widget_show(widget);
	
}

void Owned_Color_Editor::Close()
{
	if (widget == NULL)
		return; 

	value= Get_Value(); 

	gtk_object_destroy(GTK_OBJECT(widget));

	widget= NULL;
}

void Owned_Color_Editor::Signal_OK(GtkWidget *, gpointer data)
{
	Verbose_Function();
	Owned_Color_Editor &e= *(Owned_Color_Editor *)data;
	e.Close();
	e.owner().MR_Receive(e.msg); 
}

void Owned_Color_Editor::Signal_Cancel(GtkWidget *, gpointer data)
{
	Verbose_Function();
	Owned_Color_Editor &e= *(Owned_Color_Editor *)data;
	e.Close();
}

RGB Owned_Color_Editor::Get_Value() const
{
	if (widget == NULL)   return value;

	gdouble d[4];
	gtk_color_selection_get_color
		(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(widget)->colorsel),
		 d);
	RGB result((int)(d[0] * 255),
		   (int)(d[1] * 255),
		   (int)(d[2] * 255));

	return result;
}
