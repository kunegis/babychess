#include "export_options_editor.hh"

#include "cassert.hh"

bool Export_Options_Editor::OE_Open(Pointer <Widget> parent,
				    const Export_Options &options)
{
	if (! type.Open(parent, "", 1))
		return false;
	cassert (Export_Options::t_count == 2);
	const char *const text[Export_Options::t_count]=
		{
			N_("_HTML"),
			N_("_Text"),
		};
	for (int i= 0;  i < Export_Options::t_count;  ++i)
	{
		type << _(text[i]);
	}
	type.Set_Selection(options.type);
	return true;
}

bool Export_Options_Editor::OE_Retrieve(Export_Options &options)
{
	options.type= type.Selection();
	if (options.type < 0)  options.type= 0;
	return true; 
}

