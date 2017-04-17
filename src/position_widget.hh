#ifndef POSITION_WIDGET_HH
#define POSITION_WIDGET_HH

#include "text_button.hh"
#include "position_editor.hh"

//#include <memory>

class Position_Widget
	:  public H_Box
{
public:
	bool Open(Pointer <Widget> parent, String);
	void Set(String);
	String Get() const 
		{return position; }
//	const Position *Get() const
//		{ return position.get(); }
private:
	Label label;
	Text_Button button; 
//	std::auto_ptr <Position> position;
	String position; 

	Owned_Editor <Position_Editor> position_editor;

	void Wdg_Close();
	void MR_Receive(String); 
	void EW_Caption_Changed();
	void Update(); 
};

#endif /* POSITION_WIDGET_HH */
