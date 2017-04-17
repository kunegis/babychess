#include "object_editor.hh"

#include "icon.hh"

#include <gtk/gtkstock.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkvbox.h>

void Dialog::Signal_Response(GtkDialog * /* dialog */ ,
			     gint arg1,
			     gpointer user_data)
{
	Dialog &This= *(Dialog *) user_data;
	const char *message= NULL;
	switch (arg1)
	{
	case GTK_RESPONSE_APPLY:    message= "apply";  break;
	case GTK_RESPONSE_CANCEL:   message= "cancel"; break;
	case GTK_RESPONSE_OK:       message= "ok";     break;
	}
	if (message == NULL)   return;
	This.MR_Send(message); 
}


bool Dialog::Dialog_Open(bool new_hide_apply,
			 const char *ok_text)
{
	hide_apply= new_hide_apply ? 1 : 0; 

	GtkWidget *dialog= gtk_dialog_new();
	if (dialog == NULL)  return false;

	/* Buttons */
	if (! hide_apply)
		gtk_dialog_add_button(GTK_DIALOG(dialog),
				      GTK_STOCK_APPLY,
				      GTK_RESPONSE_APPLY);
	gtk_dialog_add_buttons(GTK_DIALOG(dialog),
			       GTK_STOCK_CANCEL,
			       GTK_RESPONSE_CANCEL,
			       ok_text ? ok_text : GTK_STOCK_OK,
			       GTK_RESPONSE_OK,
			       NULL);
	/* Settings */


	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

	if (! Widget::Open(NULL, dialog, true))
		return false;

	gtk_signal_connect(GTK_OBJECT(Get()), "response",
			   GTK_SIGNAL_FUNC(Signal_Response), this);

	Set_Icon(GTK_WINDOW(Get()));

	box= gtk_vbox_new(FALSE, dialog_unit);
	if (box == NULL)
	{
		Close();
		return false;
	}

	gtk_box_pack_start
		(GTK_BOX(GTK_DIALOG(Get())->vbox),
		 box,
		 TRUE,
		 TRUE,
		 0);
	gtk_container_set_border_width(GTK_CONTAINER(box), dialog_unit/2);
	gtk_widget_show(GTK_WIDGET(box));

	/* seems to have no effect */ 
//	gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),
//				       /*dialog_unit/2*/ 100);

	return true; 
}

void Dialog::Wdg_Append_Child(Widget &widget)
{
	gtk_box_pack_start
		(//GTK_BOX(GTK_DIALOG(Get())->vbox),
			GTK_BOX(box),
			widget.Get(),
		 TRUE,
		 TRUE,
			dialog_unit/2);
//	if (GTK_IS_CONTAINER(widget.Get()))
//		gtk_container_set_border_width(GTK_CONTAINER(widget.Get()), dialog_unit/2);
//	gtk_box_set_spacing(GTK_BOX(GTK_DIALOG(Get())->vbox), dialog_unit*3/2);
}
