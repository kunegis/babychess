#ifndef BOARD_IMAGE_HH
#define BOARD_IMAGE_HH

/* Original-sized version of the board image */

#include "alpha_pixmap.hh"

class Board_Image
{
public:
	static Alpha_Pixmap &Get()
		/* The big image.  Load on demand.  On failure the
		 * image may not be.  Don't change the image.  Only
		 * use its canvas.  */
		{
			if (! image.Is())  Load();
			return image;
		}

private:
	static Alpha_Pixmap image;
	static void Load();
};

#endif /* ! BOARD_IMAGE_HH */
