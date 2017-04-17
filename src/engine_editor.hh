#ifndef ENGINE_EDITOR_HH
#define ENGINE_EDITOR_HH

#include "engine.hh"
#include "conf.hh"
#include "gettext.hh"
#include "engine_stream.hh"
#include "radio_group.hh"

class Engine_Widget
	:  public V_Box
{
public:
	bool Open(Pointer <Widget> parent, const Engine &engine);
	void Update(const Engine &, bool no_message= false);
	bool Retrieve(Engine &engine); 

private:
	Framed_Radio_Group type_chooser;
	Labeled_Entry program;

	void MR_Receive(String message);
};

class Engine_Editor
	:  public Conf_Editor <Engine>
{
public:
	const char *EW_Type_Name() const  { return _("Choose an Engine");    }
	const char *Get_Type_Name() const { return "engine";                 }

private:
	Engine_Widget engine_widget; 
	Text_Button add_button, scan_button;

	bool CE_Open(Pointer <Widget> parent, const Engine &);

	bool OE_Retrieve(Engine &object); 
	void OE_Update(const Engine &engine)
		{
			engine_widget.Update(engine); 
		}
	int S_Additional_Button_Count() const 
		{ return 2; }
	void S_Append_Additional_Buttons(Table &parent);
};

#endif /* ! ENGINE_EDITOR_HH */ 
