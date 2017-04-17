#ifndef NOTATION_EDITOR_HH
#define NOTATION_EDITOR_HH

#include "conf.hh"
#include "notation.hh"
#include "check_box.hh"
#include "gettext.hh"
#include "radio_group.hh"
#include "labeled_entry_table.hh"

class Notation_Editor
	:  public Conf_Editor <Notation> 
{
public:
	const char *EW_Type_Name() const  {  return _("Choose Notation");    }
	const char *Get_Type_Name() const {  return "notation";                   }

private:

	V_Box box;
	Title_Label label;
	Labeled_Entry_Table strings;

	bool CE_Open(Pointer <Widget> parent, const Notation &);
	bool OE_Retrieve(Notation &); 
	void OE_Update(const Notation &);
};

#endif /* NOTATION_EDITOR_HH */ 
