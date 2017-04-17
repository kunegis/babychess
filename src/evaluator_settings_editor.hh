#ifndef EVALUATOR_SETTINGS_EDITOR_HH
#define EVALUATOR_SETTINGS_EDITOR_HH

#include "evaluator_settings.hh"
#include "object_editor.hh"
#include "radio_group.hh"
#include "entry.hh"
#include "check_box.hh"
#include "gettext.hh"
#include "labeled_spin_button.hh"

class Evaluator_Settings_Editor
	:  public Object_Editor <Evaluator_Settings> 
{
public:
	const char *EW_Type_Name() const
		{ return _("Engine Preferences");  }

private:
	Framed_Radio_Group /* start_chooser,*/  mode_chooser, timing_chooser; 
	Labeled_Spin_Button time, depth;
	V_Box flags_box;
	Check_Box flags[evf_count];

	bool OE_Open(Pointer <Widget> parent, const Evaluator_Settings &); 
	bool OE_Retrieve(Evaluator_Settings &);

	void MR_Receive(String);
};

#endif /* ! EVALUATOR_SETTINGS_EDITOR_HH */
