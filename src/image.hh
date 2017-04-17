#ifndef IMAGE_HH
#define IMAGE_HH

/* See etc/GDK-PIXBUF for why GdkPixbuf is not used.  (But should)  
 */

#include "pointer.hh"
#include "canvas.hh"

#include <gdk/gdkimage.h>

class Image
	:  public Pointed
{
public:

	typedef guint32 Pixel; 

	Image()
		:  image(NULL)  {  }
	bool Is() const
		{  return image != NULL;  }
	void Free();
	~Image()
		{
			if (Is())
				Free(); 
		}

	GdkPoint Size() const
		{
			assert (Is());
			return (GdkPoint)
				{image->width, image->height}; 
		}
	void Get_Pixel(int x, 
		       int y,
		       int &r,
		       int &g,
		       int &b) const
		{
			guint32 pixel= gdk_image_get_pixel(image,
							   x, y);
			r= pixel & 0xff;
			g= (pixel >> 8) & 0xff;//8
			b= (pixel >> 8) & 0xff;//8
		}

	bool Create(GdkPoint size);
	void Draw_Pixel(int x,
			int y,
			int r,
			int g,
			int b);

	bool Send_Rect(Canvas &, 
		       GdkRectangle dst,
		       GdkPoint src); 
	bool Receive_Rect(const Canvas &canvas,
			  GdkRectangle); 
	/* Must be closed.  A new client image is created which in
	 * taken from CANVAS.   */
	GdkImage *operator () ()  const {return image; }

	static bool Draw_Stretch_Canvas(Canvas &,
					const Canvas &,
					GdkRectangle &,
					GdkRectangle &);

private:
	GdkImage *image; 
}; 

#endif /* ! IMAGE_HH */
