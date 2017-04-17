#include "board_colors_editor.hh"

#include "stream_file.hh"
//#include "board_colors_reset.hh"
#include "cassert.hh"

bool Board_Colors_Editor::CE_Open(Pointer <Widget> parent,
				 const Board_Colors &settings)
{
	/* Colors */ 
	if (! color_box.Open(parent, dialog_unit/2))  return false;

	if (! label.Open(color_box, _("Colors")))
		return false;

	cassert (c_count == 8);
	static const char *const names[c_count]= 
		{ 
			N_("_Light squares"),
			N_("_Dark squares"),
			N_("Last _move"),
			N_("_Frame"),
			N_("_Selected squares"),
			N_("Fr_ame text color"),
			N_("Square _text color"),
			N_("Tutor mode “_no” square"),
		};

	for (int i= 0;  i < c_count;  ++i)
		if (! color_choosers[i].Open(color_box, 
					     settings.colors[i],
					     _(names[i])))
			return false;

	static const char *const text[bt_count] = {
		N_("R_iffled dark squares"),
	};
	for (int i= 0;  i < bt_count;  ++i)
	{
		if (! option_boxes[i].Open(color_box, settings.flags & (1 << i), _(text[i])))
			return false;
	}

	return true; 
}
    
int Board_Colors_Editor::EW_Use()
{
	return use_ok;
}

bool Board_Colors_Editor::OE_Retrieve(Board_Colors &object)
{
	object.flags= 0;
	for (int i= 0;  i < bt_count;  ++i)
		if (option_boxes[i].Checked())
			object.flags |= (1 << i);

	for (int i= 0;  i < c_count;  ++i)
		object.colors[i]= color_choosers[i].Get_Color(); 
	return true;
}

void Board_Colors_Editor::OE_Update(const Board_Colors &settings)
{
	for (int i= 0;  i < bt_count;  ++i)
		option_boxes[i].Set_Checked(settings.flags & (1 << i)); 
	for (int i= 0;  i < c_count;  ++i)
		color_choosers[i].Set_Color(settings.colors[i]);
}
