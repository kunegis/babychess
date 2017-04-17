#include "opening_settings_editor.hh"

#include "gettext.hh"

bool Opening_Settings_Editor::OE_Open(Pointer <Widget> parent, 
				      const Opening_Settings &settings)
{
	for (int i= 0;  i < op_count; ++i)
	{
		static const char *const text[op_count]=
			{N_("Show _subvariation names"),
			 N_("Show _alternative names")};
		if (! boxes[i].Open(parent, 
				    settings.flags & (1 << i),
				    _(text[i])))
			return false;
	}
	return true;
}

bool Opening_Settings_Editor::OE_Retrieve(Opening_Settings &settings)
{
	settings.flags &=~ ((1 << op_count)-1);
	for (int i= 0; i < op_count; ++i)
	{
		settings.flags |= (boxes[i].Checked() << i);
	}
	return true; 
}
