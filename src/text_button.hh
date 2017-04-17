#ifndef TEXT_BUTTON_HH
#define TEXT_BUTTON_HH

#include "widget.hh"

#include <gtk/gtkbutton.h>

class Text_Button
	:  public Widget 
{
public:

	class Open_Info
	{
	public:
		Open_Info(String new_msg_clicked= "",
			  String new_text= "",
			  bool new_sensitive= true,
			  const char *new_stock= NULL)
			:  msg_clicked(new_msg_clicked),
			   text(new_text),
			   sensitive(new_sensitive),
			   stock(new_stock)
			{  }

		String msg_clicked;
		String text;
		bool sensitive;
		const char *stock;
	};

	bool Open(Pointer <Widget> parent,
		  String msg_clicked,
		  String text= "", /* with underlined character */ 
		  bool sensitive= true,
		  const char *stock= NULL)
		{
			return Open(parent, 
				    Open_Info(msg_clicked,
					      text, 
					      sensitive,
					      stock)); 
		}
	bool Open(Pointer <Widget> parent,
		  Open_Info open_info);

private:
	String msg_clicked;

	static void Signal_Clicked(GtkButton *gtkbutton,
				   gpointer *data);
};

#endif /* ! TEXT_BUTTON_HH */ 
