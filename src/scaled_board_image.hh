#ifndef SCALED_BOARD_IMAGE_HH
#define SCALED_BOARD_IMAGE_HH

/* Scaled version of the board image as a Pixmap.   */

#include "alpha_pixmap.hh"
#include "main.hh"
#include "vector.hh"
#include "dynamic.hh"

class Scaled_Board_Image
{
public:

	Scaled_Board_Image()
		:  width(-1) 
		{ }
	~Scaled_Board_Image() 
		{ if (width >= 0)   Remove();   }

	bool Load(int width);
	/* Load from main board image with size WIDTH.  WIDTH must not
	 * be zero.  Can be called several times.   */

	Pixmap &Get_Image()
		/* Must have been loaded successfully.  Don't close it. 
		 */
		{
			assert (width != -1);
			return Get_Images()[index]().image.Get_Image();
		}

	Pixmap & Get_Alpha_Image()
		{
			assert (width != -1);
			return Get_Images()[index]().image.Get_Alpha_Image();
		}

	Alpha_Pixmap &Get_Full_Image()
		{
			assert (width != -1);
			return Get_Images()[index]().image; 
		}

	void Draw(Canvas &canvas,
		  GdkPoint origin,
		  int piece_type,
		  int chess_color);
	/* Draw piece onto canvas.  Do not draw background. 
	 */
	bool Is() const
		{  return width >= 0;  }
	int Width() const {return width; }

private:

	class Image_Info
	{
	public:

		Alpha_Pixmap image;

		int width;
		int ref_count;
	};

	int width;
	/* -1 if not loaded.
	 */
	int index;
	/* In IMAGES of THIS.  Defined if WIDTH != -1. 
	 */
	static Vector <Dynamic <Image_Info> > aa_images;

	void Remove();
	/* Remove THIS from list.  Must be loaded. 
	 */

	Canvas &Get_Canvas() 
		{
			return Get_Image()();
		}

	Canvas &Get_Alpha_Canvas()
		{
			return Get_Alpha_Image()(); 
		}
	Vector <Dynamic <Image_Info> > &Get_Images() const
		{       return aa_images; }
};

#endif /* ! SCALED_BOARD_IMAGE_HH */
