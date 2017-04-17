#include "internet_interaction_window.hh"

//#include <libintl.h>
#include "gettext.hh"

bool Internet_Interaction_Window::
Open(Pointer <Widget> parent,
     Pointer <Message_Receiver> new_owner,
     String new_msg_text,
     String new_msg_close)
{
	msg_text= new_msg_text; 
	msg_close= new_msg_close;

	if (! V_Box::Open(parent))
		return false;

	V_Box::Set_Next(true); 
	if (! internet_text.Open(this))
		goto close;

	if (! bottom_box.Open(this))
		goto close;


	bottom_box.Set_Next(true); 
	if (! entry.Open(bottom_box,
			 Entry::Open_Info("edit")))
		goto close;
	if (! check_box.Open(bottom_box,
			     true,
			     _("_Append messages")))
		goto close;

	owner= new_owner;
  
	return true;

 close:
	V_Box::Close();
	return false;
}

String Internet_Interaction_Window::Pull_Text()
{
	String result= entry.Text();
	entry.Set("");
	return result;
}

void Internet_Interaction_Window::MR_Receive(String message)
{
	if (message == "edit")
	{
		owner->MR_Receive(msg_text); 
	}
	else
		V_Box::MR_Receive(message);
}

void Internet_Interaction_Window::Wdg_Close()
{
	owner->MR_Receive(msg_close); 
	owner= NULL; 
	V_Box::Wdg_Close();
}

void Internet_Interaction_Window::Append_Message(String text)
{
	if (! check_box.Checked())  return;

	internet_text().Append_Text(text + '\n');

	internet_text().Scroll_End(); 
}
