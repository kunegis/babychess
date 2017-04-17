#ifndef BUTTON_HH
#define BUTTON_HH

#include "widget.hh"

#include <gtk/gtkbutton.h>

class Button
	:  public Widget 
{
public:
	typedef String Open_Info; /* msg_clicked */ 
	
	bool Open(Pointer <Widget> parent,
		  String new_msg_clicked);

private:

	String msg_clicked; 
	static void Signal_Clicked(GtkButton *gtkbutton,
				   gpointer *data);
};

#endif /* ! BUTTON_HH */ 
