#ifndef ASK_DIALOG_HH
#define ASK_DIALOG_HH

#include "string.hh"
#include "message_receiver.hh"
#include "widget.hh"

#include <gtk/gtkwidget.h>

class Ask_Dialog
{
public:
	Ask_Dialog()
		: dialog(NULL) 
		{}
	~Ask_Dialog()
		{ assert (dialog == NULL); }
	void Open(const char *question,
		  String msg_edited,
		  Message_Receiver &owner,
		  GtkWidget *parent,
		  const char *text_0,
		  const char *text_2,
		  const char *secondary);

	void Open(const char *question,
		  String msg_edited,
		  Widget &widget/* owner and it's window is parent */,
		  const char *text_0,
		  const char *text_2,
		  const char *secondary)
		{
			Open(question,
			     msg_edited,
			     widget,
			     widget.Root().Get(),
			     text_0,
			     text_2,
			     secondary);
		}
	void Close(); 
	int Answer()
		{
			return choice; 
		}
	bool Have_Read() { Close(); return true; }
private:	
	GtkWidget *dialog;
	String msg_edited; 
	Pointer <Message_Receiver> owner; 
	int choice; 

	static void Signal_Response(GtkWidget *, gint, gpointer);
	static gboolean Signal_Close(GtkWidget *, gpointer);
};

#endif /* ! ASK_DIALOG_HH */ 
