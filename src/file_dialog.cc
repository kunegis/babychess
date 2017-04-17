#include "file_dialog.hh"

#include "verbose.hh"
//#include "ask.hh"
#include "gettext.hh"
#include "format.hh"

#include <unistd.h>
#include <gtk/gtksignal.h>

void File_Dialog::Open(Type new_type,
		       String new_msg_edited,  
		       Widget &new_owner,
		       String filename)
{
	assert (0 <= new_type && new_type < t_count); 
	
	/* already open */ 
	if (widget != NULL)  return;

	msg_edited= new_msg_edited; 
	owner= new_owner; 
	type= new_type; 

	static const char *const text[t_count]= {N_("Open"),
						 N_("Save"),
						 N_("Insert")};

	widget= gtk_file_selection_new(_(text[type]));

	if (widget == NULL)  return; 

#if 1-1 /* put widgets here */ 
	/* could be used later for file format, encoding, etc... */ 
	GtkWidget *label= gtk_label_new("bla");
	assert (label);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(GTK_FILE_SELECTION(widget)->action_area), 
			   label,
			   TRUE, TRUE, 10);
#endif /* 1-1 */ 
			   
	gtk_window_set_transient_for(GTK_WINDOW(widget),
				     GTK_WINDOW(new_owner.Root().Get()));
	
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(widget),
					filename());

                           
	gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION(widget)->ok_button),
			    "clicked", GTK_SIGNAL_FUNC(& Signal_OK),
			    this);

	gtk_signal_connect
		(GTK_OBJECT (GTK_FILE_SELECTION(widget)->cancel_button),
		 "clicked", GTK_SIGNAL_FUNC(& Signal_Cancel),
		 this);

	gtk_signal_connect(GTK_OBJECT(widget), "destroy",
			   GTK_SIGNAL_FUNC(Signal_Destroy), this);
	gtk_widget_show(widget); 
}

void File_Dialog::Close()
{
	if (widget == NULL)
		return; 

	filename= gtk_file_selection_get_filename
		(GTK_FILE_SELECTION(widget));

	gtk_object_destroy(GTK_OBJECT(widget));

	widget= NULL;
}

void File_Dialog::Signal_OK(GtkWidget *, gpointer data)
{
	File_Dialog &fd= *(File_Dialog *)data;
	fd.Close();
	
	if (fd.type == t_save && 
	    0 == access(fd.filename(), F_OK))
	{
		String question= Format(_("A file named \"%s\" already exists."),
					fd.filename());
		fd.ask_dialog.Open
			(question(),
			 "asked",
			 fd,
			 fd.owner().Root().Get(),
			 _("_Don’t Replace"),
			 _("_Replace"),
			 /* the file */ 
			 _("Do you want to replace it with the one you are saving?"));
				   
				       return;
	}

	fd.owner().MR_Receive(fd.msg_edited); 
}

void File_Dialog::Signal_Cancel(GtkWidget *, gpointer data)
{
	File_Dialog &fd= *(File_Dialog *)data;
	fd.Close();
}

void File_Dialog::Signal_Destroy(GtkWidget *,
				 gpointer data)
{
	File_Dialog *dialog= (File_Dialog *)data;
	dialog->Close();
}

void File_Dialog::MR_Receive(String message)
{
	if (message == "asked")
	{
		int choice= ask_dialog.Answer(); 
		ask_dialog.Have_Read(); 
		switch (choice)
		{
		default:  assert(0);  break;

		case GTK_RESPONSE_YES:
//		case ask_yes:
			owner->MR_Receive(msg_edited); 
		
			break;

		case GTK_RESPONSE_NO:
			//		case ask_no:
			Open(t_save,
			     msg_edited,
			     owner(),
			     filename); 
			break;
		case GTK_RESPONSE_CANCEL:
//		case ask_cancel:
			/* do nothing */ 
			break;
		}
	}
}

