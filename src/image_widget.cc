#include "image_widget.hh"

void Image_Widget::Wdg_Draw(Canvas &canvas)
{
	GdkPoint size= Size(), image_size= {0, 0};
	canvas.Set_Function(GDK_COPY);

	if (image.Is() && image.Has_Canvas())
	{
		canvas.Draw_Canvas(image(),
				   (GdkRectangle){0, 0, size.x, size.y},
				   (GdkPoint){0,0});
		image_size= image.Get_Size();
	}

	static bool have= false;
	static GdkColor color;
	if (! have)
	{
		have= true;
		Canvas::Alloc_Color(color, RGB(0xff, 0xff, 0xff));
	}
	

	gdk_gc_set_foreground(canvas.Get_GC(), &color);
	canvas.Draw_Rect((GdkRectangle){image_size.x, 0,
			      size.x - image_size.x,
			      size.y});
	canvas.Draw_Rect((GdkRectangle){0, image_size.y,
			      image_size.x, 
			      size.y - image_size.y}); 
}

void Image_Widget::Wdg_Get_Size_Request(int &w, 
					int &h,
					bool &r)
{
	if (image.Is())
	{
		w= image.Get_Size().x;
		h= image.Get_Size().y;
	}
	else
	{
		Image_Widget_Get_Dimension(w, h);
	}

	r= false; 
}

void Image_Widget::Invalidate_Image()
{
	Image_Widget_Build(image);
	Invalidate();
	gtk_widget_queue_resize(Get());
}

void Image_Widget::Set_Image(const Pixmap &pixmap)
{
	if (Widget::Is())
	{
		Invalidate();
		gtk_widget_queue_resize(Get());
	}
	image.Free();
	if (! pixmap.Is())
		return;
	if (! image.Create(pixmap.Get_Size()))
		return;
	if (! image.Has_Canvas())
		return;
	if (! pixmap.Has_Canvas())
		return;
	image().Draw_Canvas(pixmap(),
			    (GdkRectangle){0, 0, pixmap.Get_Size().x, pixmap.Get_Size().y},
			    pixmap.Get_Size());
}

bool Image_Widget::Open(Pointer <Widget> parent)
{
	if (! Drawing_Area::Open(parent))
		return false;
	Invalidate_Image();
	return true;
}
