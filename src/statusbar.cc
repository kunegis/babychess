#include "statusbar.hh"

#include <gtk/gtkstatusbar.h>

bool Statusbar_Label::Open(Pointer <Widget> parent,
			    Open_Info open_info)
{
	if (! Widget::Open(parent, gtk_statusbar_new()))
		return false;

	gtk_statusbar_push(GTK_STATUSBAR(Get()),
			   0,
			   open_info.text());

	gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(Get()),
					  open_info.has_grip);

	return true;
}

void Statusbar_Label::Set_Text(String text)
{
	gtk_statusbar_pop(GTK_STATUSBAR(Get()),
			   0);

	gtk_statusbar_push(GTK_STATUSBAR(Get()),
			   0,
			   text());
}

bool Statusbar::Open(Pointer <Widget> parent,
		      int size)
{
	if (! H_Box::Open(parent))
		return false;

	for (int i= 0; i < size; ++i)
	{
		if (! Append("", i + 1 == size))
		{
			Close();
			return false; 
		}
	}
	return true;
}

bool Statusbar::Append(String text, bool has_grip)
{
	assert(Is());

	if (! labels.Set(*labels + 1))
		return false;

	if (! labels[-1].Alloc())
	{
		labels= -1;
		return false;
	}
	
	Statusbar_Label::Open_Info open_info= 	    {text,      has_grip};

	this->Set_Next(true); 

	if (! labels[-1]().Open
	    (this, 
	     open_info))
		return false;
	
	return true; 
}
