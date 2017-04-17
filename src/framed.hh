#ifndef FRAMED_HH
#define FRAMED_HH

#include "widget.hh"
#include "containing.hh"

#include <gtk/gtkframe.h>

class Frame
	:  public Widget
{
public:

	struct Open_Info
	{
		GtkShadowType frame_type;
		String text;

		Open_Info(GtkShadowType new_frame_type= GTK_SHADOW_IN,
			  String new_text= "")
			:  frame_type (new_frame_type),
			   text(new_text)
			{
			}
	};

	bool Open(Pointer <Widget> parent)
		{
			return Open(parent, Open_Info());
		}
	bool Open(Pointer <Widget> parent,
		  Open_Info open_info);

	void Set_Text(String text) 
		{
			gtk_frame_set_label(GTK_FRAME(Get()),
					    text());
		}
};

//#include "frame.hh"

template <class T>
class Framed
	:  public Containing <Frame, T> 
{
public:

	bool Open(Pointer <Widget> parent, 
		  typename Containing <Frame, T> ::Open_Info open_info)
		{
			return Containing <Frame, T> ::Open(parent, open_info);
		}

	bool Open(Pointer <Widget> parent)
		{
			return Containing <Frame, T> ::Open(parent);
		}

	bool Open(Pointer <Widget> parent,
		  Frame::Open_Info frame_open_info,
		  typename T::Open_Info t_open_info)
		{
			return Containing <Frame, T> ::Open
				(parent,
				 frame_open_info,
				 t_open_info);
		}

}; 

#endif /* ! FRAMED_HH */
