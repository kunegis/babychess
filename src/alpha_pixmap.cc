#include "alpha_pixmap.hh"

#include "alpha_image.hh"

bool Read_Stream(FILE *file,
		 Alpha_Pixmap *alpha_pixmap)
{
	Alpha_Image alpha_image;

	if (! Read_Stream(file, &alpha_image))
	{
		return false;
	}

	if (! (alpha_pixmap->Get_Image()
	       .Create(alpha_image.Get_Image().Size())   &&
	       alpha_pixmap->Get_Alpha_Image()
	       .Create(alpha_image.Get_Alpha_Image().Size())))
	{
		return false;
	}

	if (! (alpha_pixmap->Get_Image().Has_Canvas()   &&
	       alpha_pixmap->Get_Alpha_Image().Has_Canvas()))
	{
		return false;
	}

	alpha_image.Get_Image().Send_Rect
		(alpha_pixmap->Get_Image()(),
		 (GdkRectangle){0, 0,
					alpha_image.Get_Image().Size().x,
					alpha_image.Get_Image().Size().y},
		 (GdkPoint){0, 0});
	GdkPoint s= 	   alpha_image.Get_Alpha_Image().Size();
	alpha_image.Get_Alpha_Image().Send_Rect(alpha_pixmap->Get_Alpha_Image()(),
						(GdkRectangle){0, 0,s.x,s.y},
						(GdkPoint){0,0});

	return true;
}
