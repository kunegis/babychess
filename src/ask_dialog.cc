#include "ask_dialog.hh"

#include <gtk/gtkwindow.h>
#include <gtk/gtkmessagedialog.h>
#include <gtk/gtkstock.h>
#include <gtk/gtksignal.h>
//#include <gtk/gtkbox.h>

//#include "ask.hh"

void Ask_Dialog::Open(const char *question,
			  String new_msg_edited,
			  Message_Receiver &new_owner,
			  GtkWidget *parent,
			  const char *text_0,
			  const char *text_2,
			  const char *secondary)
{
	assert (parent);
	
	if (dialog)  return;
	msg_edited= new_msg_edited;
	owner= new_owner; 

	dialog= gtk_message_dialog_new
		(GTK_WINDOW(parent),
		 GTK_DIALOG_MODAL,
		 GTK_MESSAGE_WARNING,
		 GTK_BUTTONS_NONE,
		 question);

	if (text_2)
	{
		gtk_dialog_add_buttons
			(GTK_DIALOG(dialog),
			 text_0,           GTK_RESPONSE_NO,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 text_2,           GTK_RESPONSE_YES,
			 NULL);
		gtk_dialog_set_default_response
			(GTK_DIALOG(dialog), GTK_RESPONSE_YES);
	}
	else
	{
		gtk_dialog_add_buttons
			(GTK_DIALOG(dialog),
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 text_0,           GTK_RESPONSE_NO,
			 NULL);	
		gtk_dialog_set_default_response
			(GTK_DIALOG(dialog), GTK_RESPONSE_NO);
	}
	gtk_message_dialog_format_secondary_text(
		GTK_MESSAGE_DIALOG(dialog),
		secondary);		
			       
	gtk_signal_connect(GTK_OBJECT(dialog), "response", 
			   GTK_SIGNAL_FUNC(& Signal_Response), this);

	gtk_signal_connect(GTK_OBJECT(dialog), "close", 
			   GTK_SIGNAL_FUNC(& Signal_Close), this);


	gtk_window_set_title (GTK_WINDOW (dialog), "");
//	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);		
//	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dialog)->vbox), 12);
#if 0
	dialog= gtk_dialog_new_with_buttons 
		("",
		 /* GEdit has this empty and the HIG don't mention the
		  * title */
		 GTK_WINDOW (parent),
		 (GtkDialogFlags)GTK_DIALOG_MODAL,
		 GTK_STOCK_OK,
		 GTK_RESPONSE_OK,
		 "_Non-stock Button",
		 GTK_RESPONSE_CANCEL,
		 NULL);
//	gtk_window_set_transient_for(GTK_WINDOW(widget), GTK_WINDOW(parent));
//	Set_Icon(GTK_WINDOW(dialog));
#endif /* 0 */ 

	
	
	gtk_widget_show_all(dialog);
}

void Ask_Dialog::Close()
{
	if (dialog == NULL)  return; 
	gtk_object_destroy(GTK_OBJECT(dialog));
	dialog= NULL;
}

void Ask_Dialog::Signal_Response(GtkWidget *, gint argl, gpointer data)
{
	Verbose_Function();
	Verbose_Int(argl);
	Ask_Dialog &fd= *(Ask_Dialog *)data;
	fd.choice= argl;
	if (fd.choice == GTK_RESPONSE_DELETE_EVENT)
		fd.choice= GTK_RESPONSE_CANCEL;
	fd.owner().MR_Receive(fd.msg_edited); 
//	gtk_object_destroy(GTK_OBJECT(fd.dialog));
	fd.dialog= NULL; 
}

gboolean Ask_Dialog::Signal_Close(GtkWidget *, gpointer data)
{
	Ask_Dialog &fd= *(Ask_Dialog *)data;
	fd.dialog= NULL; 
	if (fd.choice < 0)  fd.choice= GTK_RESPONSE_CANCEL /* ask_cancel */ ;
	return FALSE;
}

