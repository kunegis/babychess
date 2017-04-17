#include "engine_editor.hh"

#include <gtk/gtkstock.h>

#define s_group_changed "s_group_changed"

bool Engine_Widget::Open(Pointer <Widget> parent, const Engine &engine)
{
	if (! V_Box::Open(parent, dialog_unit))  return false;
	
	if (! type_chooser.Open(this, s_group_changed, _("Engine type"), 1))        
		return false;
	type_chooser() << "_BabyChess"
		       << _("An _XBoard compatible engine");
	
	if (! program.Open(this, _("_XBoard engine")))  return false;

	Update(engine, true);
	
	return true;
}

void Engine_Widget::Update(const Engine &engine,
			   bool no_message)
{
	type_chooser->Set_Selection(engine.type);
	program.entry.Set(engine.program);
	if (! no_message)
		MR_Receive(s_group_changed);
}

void Engine_Widget::MR_Receive(String message)
{
	if (message == s_group_changed)
	{
		gtk_widget_set_sensitive
			(program.entry.Get(),
			 type_chooser->Selection() == et_xboard);
		Parent_Receive_Message("changed");
	}
	else
		V_Box::MR_Receive(message); 
}

bool Engine_Widget::Retrieve(Engine &engine)
{
	engine.type= type_chooser->Selection();
	if (engine.type < 0)  engine.type= 0;
	engine.program= program.entry.Text();
	return true;
}


bool Engine_Editor::CE_Open(Pointer <Widget> parent, 
			    const Engine &engine_choice)
{
	return engine_widget.Open(parent, engine_choice); 
}

bool Engine_Editor::OE_Retrieve(Engine &engine)
{
	return engine_widget.Retrieve(engine); 
}

void Engine_Editor::S_Append_Additional_Buttons(Table &parent)
{
	Verbose_Function(); 
	parent.space++;
	parent.fill= true; 
	add_button.Open(parent, "add", _("Add _XBoard Engine"));
	parent.space++;
	parent.fill= true; 
	scan_button.Open(parent, "scan", _("_Search XBoard engines"));
}

