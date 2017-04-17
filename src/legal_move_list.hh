#ifndef LEGAL_MOVE_LIST_HH
#define LEGAL_MOVE_LIST_HH

#include "list.hh"
#include "generator.hh"
#include "notation.hh"
#include "gettext.hh"
//#include "notation_settings.hh"

class Legal_Move_List
	:  public List
{
public:

	bool Open(Pointer <Widget> parent,
		  String msg_clicked= "");

	void Set(const Generator &,
		 const Notation &,
		 const Notation_Settings &);
	/* If GAME is not NULL, GENERATOR must match.  */

	void Clear()
		{
			(*this)->Set_Row_Count(0);  
		}

private:

	String msg_clicked; 

	void MR_Receive(String);
};

#endif /* ! LEGAL_MOVE_LIST_HH */
