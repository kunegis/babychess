#ifndef NOTATION_SETTINGS_EDITOR_HH
#define NOTATION_SETTINGS_EDITOR_HH

//#include "notation_settings.hh"
#include "object_editor.hh"
#include "radio_group.hh"
#include "check_box.hh"
#include "gettext.hh"
#include "notation.hh"

class Notation_Settings_Editor
	:  public Object_Editor <Notation_Settings> 
{
public:
	const char *EW_Type_Name() const  {  return _("Notation Preferences");    }
	const char *Get_Type_Name() const {  return "notation_settings";          }

private:
	V_Box option_box;
	Framed_Radio_Group type; 
	Title_Label label;
	Check_Box check_box[nos_count]; 

	bool OE_Open(Pointer <Widget> parent, const Notation_Settings &);
	bool OE_Retrieve(Notation_Settings &); 
	void OE_Update(const Notation_Settings &);
};

#endif /* NOTATION_SETTINGS_EDITOR_HH */ 
