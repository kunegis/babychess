#ifndef INTERNET_INTERACTION_WINDOW_HH
#define INTERNET_INTERACTION_WINDOW_HH

#include "box.hh"
#include "entry.hh"
#include "check_box.hh"
#include "text_view.hh"
#include "scrolled.hh"
#include "gettext.hh"

class Internet_Interaction_Window
	:  public V_Box
{
public:

	bool Open(Pointer <Widget> parent,
		  Pointer <Message_Receiver> owner,
		  String new_msg_text, /* when text is entered */ 
		  String new_msg_close);

	String Pull_Text();

	void Append_Message(String text);

private:

	H_Box bottom_box;
	Entry entry;
	Check_Box check_box;
	Scrolled <Text_View>  internet_text; 

	String msg_text, msg_close;

	Pointer <Message_Receiver> owner;

	void MR_Receive(String);
	void Wdg_Close(); 
	String Wdg_Init_Caption() const
		{ return _("Internet Chess Messages");   }
};

#endif /* ! INTERNET_INTERACTION_WINDOW_HH */ 
