#ifndef ENTRY_HH
#define ENTRY_HH

/* Entry field, entry box, ...
 */

#include "widget.hh"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkentry.h>

class Entry
	:  public Widget 
{
public:

	class Open_Info
	{
	public:

		String msg;
		String text;
		bool hidden;
		int length;
		String msg_changed; 

		Open_Info(String new_msg /* = "ok" */ ,
			  String new_text= "",
			  bool new_hidden= false,
			  int new_length= 0,
			  String new_msg_changed= "changed")
			:  msg(new_msg),
			   text(new_text),
			   hidden(new_hidden),
			   length(new_length),
			   msg_changed(new_msg_changed)
			{ }
	};

	String Text() const
		{
			assert(Is()); 
			String result= gtk_entry_get_text(GTK_ENTRY(Get()));

			return result;
		}
	bool Open(Pointer <Widget> parent,
		  Open_Info open_info);

	bool Open(Pointer <Widget> parent,
		  String text= "")
		{
			return Open(parent, 
				    Open_Info("", 
					      text));
		}
	void Set(String text)
		{
			gtk_entry_set_text(GTK_ENTRY(Get()), text());
		}
private:

	String msg, msg_changed;

	bool Wdg_Key_Press(guint);
};

#include "label.hh"
#include "box.hh"
#include "entry.hh"

class Labeled_Entry
	:  public H_Box
{
public:
	Entry entry;

	bool Open(Pointer <Widget> parent,
		  const char *label_text);

private:
	Label label;
};

#endif /* ! ENTRY_HH */
