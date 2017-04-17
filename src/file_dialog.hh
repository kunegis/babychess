#ifndef FILE_DIALOG_HH
#define FILE_DIALOG_HH

/* A filename dialog.  This class automatically asks whether a file can be
 * overwritten.   
 */

#include "widget.hh"
#include "ask_dialog.hh"

#include <gtk/gtkfilesel.h>

class File_Dialog
	:  Message_Receiver
{
public:
	enum Type {t_open, t_save, t_insert, t_count}; 

	File_Dialog()
		:  widget(NULL)
		{ }
	~File_Dialog()
		{ assert (widget == NULL);  }

	void Open(Type,
		  String msg_edited,  
		  Widget &owner,
		  String filename= "");

	void Close();

	String Filename()
		{
			if (widget == NULL)
				return filename;
			else
				return gtk_file_selection_get_filename
					(GTK_FILE_SELECTION(widget));
		}

	bool Have_Read()
		{ return true;  }
	
private:
	GtkWidget *widget; /* our widget */ 
	String filename;
	/* Valid when closed.  If open, the filename is read from the
	 * widget. */
	String msg_edited;
	Pointer <Widget> owner;
	Type type; 
	Ask_Dialog ask_dialog; 

	void MR_Receive(String); 

	static void Signal_OK(GtkWidget *, gpointer);
	static void Signal_Cancel(GtkWidget *, gpointer);
	static void Signal_Destroy(GtkWidget *, gpointer data); 
};

#endif /* ! FILE_DIALOG_HH */
