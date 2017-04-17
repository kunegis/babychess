#ifndef PIECE_RADIO_BUTTON_HH
#define PIECE_RADIO_BUTTON_HH

/* A radio button with a piece image on it.  See PROMOTION_CHOOSER for
 * usage. 
 */

#include "piece_widget.hh"
//#include "radio_group.hh"

#include <gtk/gtkbutton.h>

class Piece_Radio_Button
	:  public Widget
{
public:
	bool Open(Pointer <Widget> parent,
		  int piece,
		  String msg_changed,
		  Pointer <Widget> first);
	void Set_Checked(bool checked);

private:
	Piece_Widget piece; 
	String msg_changed;

	static void Signal_Clicked(GtkButton *gtkbutton,
				   gpointer *data);
};

#endif /* ! PIECE_RADIO_BUTTON_HH */ 
