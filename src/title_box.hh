#ifndef TITLE_BOX_HH
#define TITLE_BOX_HH

#include <gtk/gtkbox.h>

class Title_Box
	:  public V_Box
{
public:
	bool Open(Pointer <Widget> parent, 
		  const char *title)
		{
			if (! V_Box::Open(parent, dialog_unit/2))
				return false;
			label.Open(this, title);
			if (! box.Open(this, 0))
				return false;
			space.Open(box, "     ");
			return true;
		}
	/* add children here */ 
	H_Box &operator () () 
		{
			return box; 
		}
	
private:
	Title_Label label;
	H_Box box; 
	Label space;
#if 0 
	void Wdg_Append_Child(Widget &widget)
		{
			if (box.Is())
				gtk_box_pack_start(GTK_BOX(box.Get()), widget.Get(), 
						   TRUE, TRUE, 0);
			else
				V_Box::Wdg_Append_Child(widget); 
		}
#endif /* 0 */ 
};

#endif /* ! TITLE_BOX_HH */ 




