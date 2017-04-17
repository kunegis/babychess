#include "pixmap_stream.hh"

/* Write/read a PIXMAP to/from a file by using a temporary
 * IMAGE. */

#include "image_stream.hh"

bool Write_Stream(FILE *file, 
		  const Pixmap *pixmap)
{
	assert (pixmap->Is());

	if (! pixmap->Has_Canvas())
		return false;

	Image image;
	if (! image.Receive_Rect
	    ((*pixmap)(), 
	     (GdkRectangle)
	    {0, 0, pixmap->Get_Size().x, pixmap->Get_Size().y}))
		return false;

	return Write_Stream(file, &image);
}

bool Read_Stream(FILE *file,
		 Pixmap *pixmap)
{
	Image image;

	if (! Read_Stream(file, &image))
		return false;

	if (! pixmap->Create(image.Size()))
		return false;

	if (! pixmap->Has_Canvas())
		return false;

	image.Send_Rect
		(pixmap->Get_Canvas(),
		 (GdkRectangle){0, 0, image.Size().x,image.Size().y},
		 (GdkPoint){0,0});

	return true;
}
