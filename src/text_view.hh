#ifndef TEXT_VIEW_HH
#define TEXT_VIEW_HH

#include "string.hh"
#include "widget.hh"

#include <gtk/gtktextbuffer.h>

class Text_View
	:  public Widget 
{
public:

	struct Open_Info
	{
		String text;
		bool editable;
		const char *msg_changed;

		Open_Info(String new_text= "",
			  bool new_editable= false,
			  const char *new_msg_changed= NULL)
			:  text (new_text),
			   editable (new_editable),
			   msg_changed(new_msg_changed)
			{ }
	};

	bool Open(Pointer <Widget> parent,
		  String text= "",
		  bool editable= false,
		  const char *msg_change= NULL);
	bool Open(Pointer <Widget> parent,
		  Open_Info open_info)
		{
			return Open(parent, 
				    open_info.text,
				    open_info.editable,
				    open_info.msg_changed); 
		}

	void Append_Text(String text);

	void Wdg_Get_Size_Request(int &width,
				  int &height,
				  bool &resizeable);

	void Scroll_End();

	void Clear();
	String Get_Text() const;
	void Set_Text(String text)
		{
			Clear();
			Append_Text(text); 
		}

private:
	const char *msg_changed;
	
	static void Signal_Changed(GtkTextBuffer *,
				   gpointer);
};

#endif /* ! TEXT_VIEW_HH */ 
