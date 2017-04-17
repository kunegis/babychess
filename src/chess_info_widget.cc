#include "chess_info_widget.hh"

#include <algorithm>

void Chess_Info_Widget::Wdg_Get_Size_Request(int &width,
					     int &height,
					     bool &resizeable)
{
	width= height= dialog_unit;
	resizeable= false;
}

void Chess_Info_Widget::Wdg_Draw(Canvas &canvas)
{
	switch (chess_color)
	{
	case cc_w:
	case cc_b:
	{
		int w= std::max(dialog_unit/4, 1);

		GdkColor black;
		gdk_color_black(gdk_colormap_get_system(), &black);

		gdk_gc_set_foreground(canvas.Get_GC(), &black); 
		canvas.Draw_Rect((GdkRectangle){w, w, Size().x-2*w, Size().y-2*w});
		if (! chess_color)
		{
			GdkColor white;
			gdk_color_white(gdk_colormap_get_system(), &white);
			
			gdk_gc_set_foreground(canvas.Get_GC(), &white); 
			w += std::max(dialog_unit/15*5/3, 1);
			canvas.Draw_Rect((GdkRectangle){w, w, Size().x-2*w, Size().y-2*w});
		}
		break;
	}
		
	case t_empty:
		break;

	case t_up:
	case t_down:
		int w= std::max(dialog_unit*5/3/10, 1);
		if (! have_color)
		{
			have_color= true;
			Canvas::Alloc_Color(color, RGB(0x00, 0xFF, 0x00)); 
		}
		GdkColor black;
		gdk_color_black(gdk_colormap_get_system(), &black);

		gdk_gc_set_foreground(canvas.Get_GC(), &black); 
		canvas.Draw_Ellipse((GdkRectangle){w, w, Size().x-2*w, Size().y-2*w});
		gdk_gc_set_foreground(canvas.Get_GC(), &color);
		w += std::max(dialog_unit/12, 1);
		canvas.Draw_Ellipse((GdkRectangle){w, w, Size().x-2*w, Size().y-2*w});
		break;
	}
}

void Chess_Info_Widget::Set(int new_color)
{
	chess_color= new_color; 
	Invalidate(); 
}

bool Chess_Info_Widget::Open(Pointer <Widget> parent,
			     int new_chess_color)
{
	chess_color= new_chess_color;
	have_color= false;
	return Drawing_Area::Open(parent); 
}
