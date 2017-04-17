#ifndef SPIN_BUTTON_HH
#define SPIN_BUTTON_HH

#include "widget.hh"

#include <gtk/gtkeditable.h>
#include <gtk/gtkspinbutton.h>

class Spin_Button
	:  public Widget 
{
public:

	struct Open_Info
	{
		const char *msg_changed; 
		//		String msg_changed;
		int value, min, max; 
	};

	int Value() const
		{
			return gtk_spin_button_get_value_as_int
				(GTK_SPIN_BUTTON(Get()));
		}
	void Set(int);
	bool Open(Pointer <Widget> parent,
		  Open_Info open_info);

private:
	GtkAdjustment *adjustment;
	String msg_changed;
	bool in;

	void Wdg_Close();

	static void Signal_Changed(GtkEditable *editable,
				   gpointer user_data);
};

#endif /* ! SPIN_BUTTON_HH */
