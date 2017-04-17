#ifndef PIXMAP_HH
#define PIXMAP_HH

#include "canvas.hh"
#include "widget.hh"

class Pixmap
{
public:

	Pixmap()
		:  pixmap(NULL)  { }
	bool Is() const { return pixmap != NULL;  }
	~Pixmap()
		{  if (Is())  Free();   }


	Canvas &Get_Canvas() const
	/* Has_Canvas() must have returned TRUE.  */
		{
			assert (canvas.Is());
			return ((Pixmap *)this)->canvas; 
		}
	Canvas &operator () () const
		{ return Get_Canvas();  }
	Canvas *operator->() const
		{ return &Get_Canvas(); }

	GdkPoint Get_Size() const
	/* Image must have been created. */
		{
			assert (Is()); 
			return size; 
		}

	void Free();
	/* Do nothing if image is not created.  */

	bool Create(GdkPoint new_size);
	/* Neither X nor Y of SIZE must be 0.  Return TRUE on success.
	 * Must not be allocated. 
	 */

	bool Has_Canvas() const
	/* Whether a canvas could be created. */
		{
			assert (Is()); 
			if (canvas.Is())
				return true;
			return ((Pixmap *)this)->Open_GC();
		}

private:

	GdkPixmap *pixmap;
	GdkGC     *gc;
	Canvas     canvas;
	GdkPoint   size;

	bool Open_GC();
};

#endif /* ! PIXMAP_HH */ 
