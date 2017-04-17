#include "legal_move_list.hh"

#include "format.hh"
#include "format_move_name.hh"

void Legal_Move_List::Set(const Generator &generator,
			  const Notation &notation,
			  const Notation_Settings &notation_settings)
{
	(*this)->Set_Row_Count(*generator);
	for (int i= 0;  i < *generator;  ++i)
		(*this)->Set
			(0, i, 
			 Format_Move_Name
			 (generator.Get_Move_Name(i),
			  notation,
			  notation_settings));
}

bool Legal_Move_List::Open(Pointer <Widget> parent,
			   String new_msg_clicked)
{
	msg_clicked= new_msg_clicked;

	if (! List::Open(parent, List::Open_Info
			 (1, scr_vertical, "list_clicked")))  
		return false;
	
	return true; 
}

void Legal_Move_List::MR_Receive(String message)
{
	if (message == "list_clicked")
	{
		if ((*this)->Double_Click())  MR_Receive(msg_clicked);
	}
	else
		List::MR_Receive(message);
}
