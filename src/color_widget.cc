#include "color_widget.hh"

void Color_Widget::Wdg_Draw(Canvas &canvas)
{
	GdkPoint size= Size();

	if (! have)
	{
		have= true;
		Canvas::Alloc_Color(gdkcolor, rgb);
	}
	gdk_gc_set_foreground(canvas.Get_GC(), &gdkcolor);
	canvas.Draw_Rect((GdkRectangle){0, 0, size.x, size.y});

	switch (glyph)
	{
	default: case g_no:  break;

	case g_pause:
		GdkColor black;
		gdk_color_black(gdk_colormap_get_system(), &black);
		gdk_gc_set_foreground(canvas.Get_GC(), &black);
		canvas.Draw_Rect((GdkRectangle){size.x / 5, size.y / 5, 
				      size.x / 5, size.y * 3 / 5});
		canvas.Draw_Rect((GdkRectangle){size.x * 3 / 5, size.y / 5, 
				      size.x / 5, size.y * 3 / 5});
		break;
	}
}

bool Color_Widget::Open(Pointer <Widget> parent, 
			RGB new_rgb,
			Glyph new_glyph)
{
	if (! Drawing_Area::Open(parent))  return false;

	rgb= new_rgb;
	glyph= new_glyph;
	have= false;

	return true;
}

void Color_Widget::Set_Color(RGB new_rgb)
{
	rgb= new_rgb;
	have= false;
	Invalidate();
}

void Color_Widget::Set_Glyph(Glyph new_glyph)
{
	glyph= new_glyph; 
	Invalidate(); 
}

void Color_Widget::Wdg_Get_Size_Request(int & width,
					int &  height,
					bool & resizeable)
{
	width= height= dialog_unit*4/3;
	resizeable= false;
}
