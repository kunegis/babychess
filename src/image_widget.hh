#ifndef IMAGE_WIDGET_HH
#define IMAGE_WIDGET_HH

#include "pixmap.hh"
#include "drawing_area.hh"

class Image_Widget
	:  public Drawing_Area
{
public:

	class Open_Info  { };

	bool Open(Pointer <Widget> parent);

	void Wdg_Get_Size_Request(int &, int &, bool &);

	const Pixmap &Get_Image() const
		{  return image;  }

	void Set_Image(const Pixmap &pixmap);

protected:
	void Invalidate_Image();
	/* Called by subclasses when the images must be redrawn.
	 * Image_Widget_Build() will be called. 
	 */

private:

	Pixmap image;
	/* Can be changed by inheriting.  May be set even when Widget
	 * is not open. 
	 */

	virtual void Image_Widget_Build(Pixmap & /* pixmap */)
		/* Load something into PIXMAP.  PIXMAP is closed and
		 * must be opened.  */
		{ }
	virtual void Image_Widget_Get_Dimension(int &width,
						int &height)
		/* Dimension if PIXMAP is cleared.  */
		{
			width= height= 0;
		}

	void Wdg_Draw(Canvas &);
};

#endif /* ! IMAGE_WIDGET_HH */
