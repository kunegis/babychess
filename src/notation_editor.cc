#include "notation_editor.hh"

#include "verbose.hh"
#include "stream_file.hh"
#include "gettext.hh"
#include "cassert.hh"

bool Notation_Editor::CE_Open(Pointer <Widget> parent,
			      const Notation &notation)
{
	/* Strings */ 
	if (! box.Open(parent))  return false;
	cassert (i_count == 15);
	static const char *const texts[i_count]= 
		{
			N_("_En passant"), 
			N_("_Short castling"), 
			N_("_Long castling"), 
			N_("_Pawn"),
			N_("K_night"),
			N_("_Bishop"), 
			N_("_Rook"),
			N_("_Queen"),
			N_("_King"), 
			N_("_Capture"),
			N_("_Promotion"),
			N_("_White"),
			N_("_Black"),
			N_("_Check"),
			N_("_File characters"),
		};
		
	if (! label.Open(box, _("Symbols")))  return false;
	if (! strings.Open(box, i_count, texts))  return false;
	for (int i= 0;  i < i_count;  ++i)
		strings.Set(i, notation.texts[i]);

	return true; 
}

bool Notation_Editor::OE_Retrieve(Notation &notation)
{
	/* Strings */
	for (int i= 0;  i < i_count;  ++i)
		notation.texts[i]= strings[i];
	return true; 
}

void Notation_Editor::OE_Update(const Notation &notation)
{
	for (int i= 0;  i < i_count; ++i)
		strings.Set(i, notation.texts[i]);
}

