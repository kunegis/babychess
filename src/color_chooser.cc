#include "color_chooser.hh"

#include <gtk/gtklabel.h>

#define s_edit "edit"
#define s_edited "edited"

bool Color_Chooser::Open(Pointer <Widget> parent,
			 RGB new_color,
			 String text)
{
	color= new_color;

	if (! Button::Open(parent, s_edit))
		return false;
	if (! box.Open(this, dialog_unit/2))
		goto close;

//	if (! H_Box::Open(parent, dialog_unit/2))  return false;


	if (! color_widget./*Buttoned <Color_Widget> ::*/Open
	    (box, color)) return false;
	if (! label.Open(box, text, 0.0, "", true))  return false;
	gtk_label_set_mnemonic_widget(GTK_LABEL(label.Get()), this->Get());

	return true; 
 close:
	Close();
	return true;
}

void Color_Chooser::Wdg_Close()
{
	editor.Close();
	Button::Wdg_Close();
}

void Color_Chooser::MR_Receive(String message)
{
	if (message == s_edited)
	{
		RGB new_color= editor.Get_Value();
		if (color == new_color) return;
		color= new_color;
		color_widget.Set_Color(color);
		editor.Have_Read(); 
		Parent_Receive_Message("changed");
	}
	else if (message == s_edit)
	{
		editor.Open(s_edited, 
			    *this,
			    RGB(color));
	}

	else 
		Button::MR_Receive(message);
}

void Color_Chooser::Set_Color(RGB new_color)
{
	editor.Close(); 
	color= new_color; 
	color_widget.Set_Color(new_color); 
}
