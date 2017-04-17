#ifndef OWNED_COLOR_EDITOR_HH
#define OWNED_COLOR_EDITOR_HH

#include "widget.hh"
#include "rgb.hh"

class Owned_Color_Editor
{
public:

	Owned_Color_Editor()
		:  widget(NULL)
		{ }
	~Owned_Color_Editor()
		{  assert (widget == NULL);   }

	void Open(String msg,
		  Widget &widget)
		{  return Open(msg, widget, RGB(0, 0, 0)); }
	void Open(String msg,
		  Widget &, 
		  const RGB &);
	void Close();

	RGB Get_Value() const;

	bool Have_Read()
		{  return true;  }
private:
	String msg;
	GtkWidget *widget;
	Pointer <Widget> owner; 
	RGB value;

	static void Signal_OK(GtkWidget *, gpointer);
	static void Signal_Cancel(GtkWidget *, gpointer);
}; 

#endif /* ! OWNED_COLOR_EDITOR_HH */ 
