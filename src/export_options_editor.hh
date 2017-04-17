#ifndef EXPORT_OPTIONS_EDITOR_HH
#define EXPORT_OPTIONS_EDITOR_HH

#include "export_options.hh"
#include "object_editor.hh"
#include "gettext.hh"
#include "radio_group.hh"

class Export_Options_Editor
	:  public Object_Editor <Export_Options>
{
public:
	Radio_Group type;
	
	const char *EW_Type_Name() const  
		{  /* Verb */ return _("Export");   }
	bool OE_Open(Pointer <Widget> parent, const Export_Options &); 
	bool OE_Retrieve(Export_Options &);
}; 

#endif /* ! EXPORT_OPTIONS_EDITOR_HH */ 

