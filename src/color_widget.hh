#ifndef COLOR_WIDGET_HH
#define COLOR_WIDGET_HH

#include "rgb.hh"
#include "drawing_area.hh"

class Color_Widget
	:  public Drawing_Area
{
public:

	enum Glyph {g_no, g_pause};

	typedef RGB Open_Info;

	bool Open(Pointer <Widget> , RGB= RGB(), Glyph= g_no);
	void Set_Color(RGB);
	void Set_Glyph(Glyph= g_no);

private:

	RGB rgb;
	Glyph glyph;
	
	GdkColor gdkcolor; 
	bool have;

	void Wdg_Get_Size_Request(int & /* width */ ,
				  int & /* height */ ,
				  bool & /* resizeable */ );
	void Wdg_Draw(Canvas &);
};

#endif /* ! COLOR_WIDGET_HH */
