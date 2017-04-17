#include "promotion_chooser.hh"

#include "gettext.hh"

#include <gtk/gtktogglebutton.h>

bool Promotion_Chooser::Open(Pointer <Widget> parent)
{
	if (! H_Box::Open(parent, dialog_unit/2))  return false; 

	for (int i= 0;  i < 4;  ++i)
	{
		if (! buttons[i].Open(this, Piece(cc_w, pi_Q-i),
				      /*Format("clicked %d", i)*/"",
				      i?&buttons[0]:(Widget*)NULL))  
			goto close;
		gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(buttons[i].Get()),
					   FALSE); 
	}

	return true;

 close:
	Close(); 
	return false;
}

int Promotion_Chooser::Selection()
{
	for (int i= 0; i < 4; ++i)
	{
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttons[i].Get())))
		{
			/* Set the selection to the queen */ 
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttons[0].Get()),true);
			return i;
		}
	}
	return 0;
}

void Promotion_Chooser::Update(const Position &position)
{
	if (GTK_WIDGET_VISIBLE(Get()))  return; 

	for (int file= 0;  file < 8;  ++file)
	{
		if (position[Make_Square(file, sq_6)] == pi_wP  ||
		    position[Make_Square(file, sq_7)] == pi_wP  ||
		    position[Make_Square(file, sq_3)] == pi_bP  ||
		    position[Make_Square(file, sq_2)] == pi_bP)
		{
			Set_Visible(true);
			return; 
		}
	}
}
