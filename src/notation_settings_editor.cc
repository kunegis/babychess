#include "notation_settings_editor.hh"

#include "cassert.hh"

bool Notation_Settings_Editor::OE_Open(Pointer <Widget> parent,
				       const Notation_Settings &notation)
{
	/* options */ 
	if (! option_box.Open(parent))        return false;
	if (! type.Open(option_box, "type", _("Notation type"), 1))  return false;
	cassert (n_count == 2);
	type() << _("_Short")
	       << _("_Long");
	type->Set_Selection(notation.type);
	if (! label.Open(option_box, _("Options")))
		return false;
	for (int i= 0;  i < nos_count;  ++i)
	{
		cassert (nos_count == 8);
		static const char *const text[nos_count]=
			{N_("Show the _pawn sign"),
			 N_("Don’t indicate _en passant captures"),
			 N_("Don’t show _check and mate"),
			 N_("No _capture sign in short notation"),
			 N_("Capture sign at the _end in short notation"),
			 N_("Display the _captured piece in long notation"),
			 N_("Don’t indicate _double check"),
			 N_("Don’t use _castling symbols")
			};
			 
		if (! check_box[i].Open(option_box,
					notation.flags & (1 << i),
					_(text[i])))
			return false; 
	}

	return true; 
}

bool Notation_Settings_Editor::OE_Retrieve(Notation_Settings &notation)
{
	/* Type */
	notation.type= type->Selection();
	assert (notation.type >= 0);

	/* Options */ 
	notation.flags= 0;
	for (int i= 0;  i < nos_count;  ++i)
		if (check_box[i].Checked())  
			notation.flags |= (1 << i);
	return true;
}

void Notation_Settings_Editor::OE_Update(const Notation_Settings &notation)
{
	type->Set_Selection(notation.type); 

	for (int i= 0;  i < nos_count;  ++i)
		check_box[i].Set_Checked(notation.flags & (1 << i/* << nt_base*/)); 
}

