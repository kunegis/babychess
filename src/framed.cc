#include "framed.hh"

bool Frame::Open(Pointer <Widget> parent,
		 Open_Info open_info)
{
	if (! Widget::Open(parent, gtk_frame_new(NULL)))
		return false;
	gtk_frame_set_shadow_type(GTK_FRAME(Get()),
				  open_info.frame_type); 
	if (open_info.text.Is())
		gtk_frame_set_label(GTK_FRAME(Get()),
				    open_info.text());
	return true;
}
