#ifndef COLOR_CHOOSER_HH
#define COLOR_CHOOSER_HH

#include "owned_color_editor.hh"
#include "text_button.hh"
#include "box.hh"
#include "label.hh" 
#include "color_widget.hh"
#include "buttoned.hh"

class Color_Chooser
	:  public Button
//	:  public H_Box
{
public:

	bool Open(Pointer <Widget>,
		  RGB,
		  String);
	RGB Get_Color() const  
		{  return color; }
	void Set_Color(RGB new_color); 

private:
	H_Box box;
	Color_Widget color_widget;
	Label label;
///	Buttoned <Color_Widget> button;
	

	Owned_Color_Editor editor; 
	RGB color;

	void MR_Receive(String);
	void Wdg_Close();
};

#endif /* ! COLOR_CHOOSER_HH */
