#include "position_widget.hh"

#include "fen.hh"
//#include "parse_fen.hh"

bool Position_Widget::Open(Pointer <Widget> parent, String new_position)
{
	if (! H_Box::Open(parent))
		return false; 
	H_Box::Set_Next(true); 
	if (! label.Open(this))
		goto close;
	if (! button.Open(this, "click", _("_Change")))
		goto close; 
	position= new_position; 
	Update();
	return true;
 close:
	return false;
}

void Position_Widget::Set(String new_position)
{
	position= new_position; 
	Update(); 
}

void Position_Widget::Update()
{
	label.Set(Format("%s  %s",
			 _("Starting position:"),
			 position ? _("custom") : _("default")));
}

void Position_Widget::MR_Receive(String message)
{
	if (message == "click")
	{
		Position p;
		if (position)
		{
			bool pf= Parse_FEN(position, p);
			assert (pf); 
		}
		else
			p.Reset();
		position_editor.Open("edited", 
				     *this,
				     p,
				     this,
				     _("Custom Position"));
	}
	else if (message == "edited")
	{
		position= FEN(position_editor.Value());
		position_editor.Have_Read();
		Update();
	}
	else
		H_Box::MR_Receive(message); 
}

void Position_Widget::Wdg_Close()
{
	position_editor.Close();
	H_Box::Wdg_Close(); 
}

void Position_Widget::EW_Caption_Changed()
{
	position_editor.Update_Caption();
}

