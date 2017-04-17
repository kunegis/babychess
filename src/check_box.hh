#ifndef CHECK_BOX_HH
#define CHECK_BOX_HH

#include "verbose.hh"
#include "widget.hh"

#include <glib/gtypes.h>
#include <gtk/gtkbutton.h>

class Check_Box
	:  public Widget 
{
public:
	bool Open(Pointer <Widget> parent,
		  bool checked,
		  String text,
		  String msg_clicked= "changed");
	
	void Set_Checked(bool);
	bool Checked() const;

private:
	String msg_clicked;

	static void Signal_Clicked(GtkButton *gtkbutton,
				   gpointer *data);
};

#endif /* ! CHECK_BOX_HH */
