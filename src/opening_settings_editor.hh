#ifndef OPENING_SETTINGS_EDITOR_HH
#define OPENING_SETTINGS_EDITOR_HH

#include "object_editor.hh"
#include "check_box.hh"
#include "opening_settings.hh"
#include "gettext.hh"

class Opening_Settings_Editor
	:  public Object_Editor <Opening_Settings>
{
public:
	bool OE_Open(Pointer <Widget> parent, const Opening_Settings &); 
	const char *Get_Type_Name() const { return "opening_settings"; }
	const char *EW_Type_Name() const  { return _("Opening Preferences");    }

private:
	Check_Box boxes[op_count];

	bool OE_Retrieve(Opening_Settings &object);
};

#endif /* ! OPENING_SETTINGS_EDITOR_HH */ 
