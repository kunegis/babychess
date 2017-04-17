#ifndef STATUSBAR_HH
#define STATUSBAR_HH

#include "box.hh"
#include "dynamic.hh"

#include <gtk/gtkstatusbar.h>

class Statusbar_Label
	:  public Widget
{
public:

	struct Open_Info
	{
		String text;
		bool has_grip;
	};

	bool Open(Pointer <Widget> parent,
		  Open_Info open_info);

	void Set_Text(String text);
};

class Statusbar
	:  public H_Box
{
public:

	bool Open(Pointer <Widget> parent,
		  int size= 0);

	void Set(int n, String text)
		{  labels[n]().Set_Text(text);  }
	GtkStatusbar *Get(int n) {return GTK_STATUSBAR(labels[n]().Get()); }
	void Set_Sensitive(int i, bool sensitive)
		{
			gtk_widget_set_sensitive(labels[i]().Get(), sensitive);
		}

private:

	Vector <Dynamic <Statusbar_Label> > labels;

	bool Append(String text, bool has_grip);
};

#endif /* ! STATUSBAR_HH */
