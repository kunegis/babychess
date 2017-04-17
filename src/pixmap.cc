#include "pixmap.hh"

void Pixmap::Free()
{
	if (! Is())
		return; 

	if (canvas.Is())
	{
		canvas.Close();
		gdk_gc_unref(gc); 
	}
	gdk_pixmap_unref(pixmap);
	pixmap= NULL; 
}


bool Pixmap::Open_GC()
{
	assert (gc == NULL);
	gc= gdk_gc_new(pixmap); 
	if (gc == NULL)
		return false;
	canvas.Open(pixmap, gc); 
	return true;
}

bool Pixmap::Create(GdkPoint new_size)
{
	assert (new_size.x > 0 && new_size.y > 0); 
	assert (! Is()); 

	pixmap= gdk_pixmap_new(NULL,
			       new_size.x, new_size.y,
			       Widget::Get_Depth());
	if (pixmap == NULL)
		return false;
	gc= NULL; 
	size= new_size; 
	return true; 
}
