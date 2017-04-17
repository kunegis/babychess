#ifndef ALPHA_IMAGE_HH
#define ALPHA_IMAGE_HH

#include "image.hh"
#include "pixmap.hh"

class Alpha_Image
{
public:

	bool Is() const
		{  return image.Is();  }

	void Clear()
		{
			image.Free();
			alpha.Free();
		}

	GdkPoint Size() const
		{
			assert (Is());
			return image.Size();
		}

	void Draw(Pixmap &,
		  GdkPoint);

	Image &Get_Image() 
		{
			return image;
		}

	Image &Get_Alpha_Image()
		{
			return alpha;
		}

private:
	Image image;
	Image alpha;
	/* transparent is black, opaque is white.  */
};

bool Read_Stream(FILE *, Alpha_Image *); 

#endif /* ! ALPHA_IMAGE_HH */
