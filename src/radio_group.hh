#ifndef RADIO_GROUP_HH
#define RADIO_GROUP_HH

#include "box.hh"
#include "format.hh"
#include "dynamic.hh"
#include "label.hh"
#include "title_box.hh"

#include <gtk/gtkbutton.h>

class Text_Radio_Button
	:  public Widget
{
public:
	bool Open(Pointer <Widget> parent,
		  String text,
		  String msg_changed,
		  Pointer <Widget> first);

	void Set_Checked(bool checked);
	void Set_Text(String); 

private:
	String msg_changed;

	static void Signal_Clicked(GtkButton *gtkbutton,
				   gpointer *data);
};

class Radio_Group
	:  public Table
{
public:

	class Open_Info
	{
	public:

		String msg_changed;
		int columns;

		Open_Info(String new_msg_changed, 
			  int new_columns)
			:  msg_changed (new_msg_changed), 
  			   columns (new_columns) 
			{ }
	};

	bool Open(Pointer <Widget> parent, Open_Info);
	bool Open(Pointer <Widget> parent, 
		  String new_msg_changed, 
		  int new_columns)
		{
			return Open(parent,
				    Open_Info(new_msg_changed,
					      new_columns)); 
		}
	Radio_Group &operator << (String text);
	/* Add a radio button with text only */

	int Next_ID()
		{
			return *buttons;
		}
	int Selection() const
		/* -1 when no selection */
		{ return selection;  }

	void Set_Selection(int new_selected);

	int Count() const
		{  return Get_Children_Count();  }
	void Set_Text(int, String);

private:

	int selection;
	/* Item selected or -1.
	 */

	String msg_changed;
	/* Sent on change.
	 */

	int columns;
	/* Number of colums. 
	 */

	Vector <Dynamic <Text_Radio_Button> > buttons;

	void MR_Receive(String);
};

class Framed_Radio_Group
	:  public Title_Box
{
public:
	bool Open(Pointer <Widget> parent,
		  const char *msg_change,
		  const char *text, /* translated and no markup */ 
		  int columns= 1)
		{
			if (! Title_Box::Open(parent, text))
				return false;
//			if (! label.Open(this, text))  goto close;
			if (! radio_group.Open(((Title_Box &)(*this))(), msg_change, columns))
				goto close; 
			return true; 
		close:
			Close(true);
			return false;
		}
	Radio_Group &operator () () 
		{ return radio_group; }
	Radio_Group *operator -> ()
		{ return & radio_group; }

//	Title_Label label;
	Radio_Group radio_group;
};

#endif /* ! RADIO_GROUP_HH */
