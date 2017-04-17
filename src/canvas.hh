#ifndef CANVAS_HH
#define CANVAS_HH

#include "rgb.hh"

#include <gdk/gdkcolor.h>
#include <gdk/gdkgc.h>
#include <gdk/gdkdrawable.h>
#include <gdk/gdkfont.h>

class Canvas
{
public:
	Canvas()
		:  drawable (NULL),   gc(NULL)   { }

	Canvas(GdkDrawable *new_drawable,
	       GdkGC       *new_gc)
		:  drawable(new_drawable),
		   gc(new_gc)
		{ }
	bool Is() const
		{
			assert ((drawable == NULL) != (gc != NULL)); 
			return drawable != NULL; 
		}
	void Open(GdkDrawable *new_drawable,
		  GdkGC       *new_gc)
		{
			assert (drawable == NULL);
			assert (gc == NULL);
			drawable= new_drawable;
			gc= new_gc;
		}
	void Close()
		{
			assert (Is()); 
			drawable= NULL;
			gc= NULL; 
		}

	/* Everything is drawn in foreground color.  */
	void Draw_Rect(GdkRectangle r)
		{
			gdk_draw_rectangle(drawable,
					   gc,
					   TRUE,
					   r.x,
					   r.y,
					   r.width,
					   r.height); 
		}

	void Set_Line_Mode(int width,
			   GdkJoinStyle join_style= GDK_JOIN_MITER,
			   GdkCapStyle cap_style= GDK_CAP_NOT_LAST,
			   GdkLineStyle line_style= GDK_LINE_SOLID)
		{
			gdk_gc_set_line_attributes(gc,
						   width,
						   line_style,
						   cap_style, join_style); 
					     
		}

	void Draw_Stretch_Canvas(const Canvas &canvas,
				 GdkRectangle dst,
				 GdkRectangle src);
	void Set_Function(GdkFunction function);

	void Draw_Canvas(const Canvas & /* canvas */ ,
			 GdkRectangle /* dst */ ,
			 GdkPoint /* src */ );

	void Draw_Ellipse(GdkRectangle); 
	void Draw_Label(GdkPoint point,  /*String text*/ const char *text, int text_length); 


	GdkDrawable *Get_Drawable() const
		{ return drawable;  }
	GdkGC *Get_GC() const
		{  return gc;   }

	static int Get_Ascent()
		{
			if (! Load_Font())  return 1;
			return font->ascent;
		}
	static int Get_Descent()
		{
			if (! Load_Font())  return 1;
			return font->descent; 
		}

	static int Get_Label_Width(const char *text, int text_length);

	static void Alloc_Color(GdkColor &color, RGB rgb);
	
private:
	GdkDrawable *drawable;
	GdkGC       *gc; 

	static GdkFont *font;

	static bool Load_Font();
};



#endif /* ! CANVAS_HH */ 
