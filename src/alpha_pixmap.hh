#ifndef ALPHA_PIXMAP_HH
#define ALPHA_PIXMAP_HH

#include "pixmap.hh"

class Alpha_Pixmap
	:  virtual public Pointed
{
public:

	bool Is() const {  return image.Is();  }

	Pixmap &Get_Image()
		{
			return image;
		}

	Pixmap &Get_Alpha_Image()
		{
			return alpha_image;
		}

private:

	Pixmap image;
	Pixmap alpha_image;
};

bool Read_Stream(FILE *, Alpha_Pixmap *); 

#endif /* ! ALPHA_PIXMAP_HH */
