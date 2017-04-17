#include "alpha_image.hh"

#include "png.hh"
#include "cassert.hh"

void Alpha_Image::Draw(Pixmap &si,
		       GdkPoint p)
{
	cassert (Is());
	assert (si.Is());
	
	if (! si.Has_Canvas())
		return;

	Pixmap s1;

	if (! s1.Create(Size()))
		return;

	if (! s1.Has_Canvas())
		return;
	
	image.Send_Rect(s1(),
			(GdkRectangle){0, 0, Size().x, Size().y},
			(GdkPoint){0,0});
	
	Pixmap s2;

	if (! s2.Create(Size()))
		return;

	if (! s2.Has_Canvas())
		return;

	alpha.Send_Rect(s2(),
			(GdkRectangle){0, 0, Size().x, Size().y},
			(GdkPoint){0,0});
	


	si().Set_Function(GDK_AND_INVERT);
	si().Draw_Canvas(s2(),
			 (GdkRectangle){p.x,p.y,Size().x, Size().y},
			 (GdkPoint){0,0});

	si().Set_Function(GDK_OR);
	si().Draw_Canvas(s1(),
			 (GdkRectangle){p.x,p.y,Size().x, Size().y},
			 (GdkPoint){0,0});
}

bool Read_Stream(FILE *file,
		 Alpha_Image *alpha_image)
{
	alpha_image->Clear();

	return PNG::Read(file,
			 alpha_image->Get_Image(),
			 & alpha_image->Get_Alpha_Image());
}
