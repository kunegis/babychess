#include "canvas.hh"

#include "image.hh"
#include "verbose.hh"

#include <limits.h> /* ULONG_MAX */

GdkFont *Canvas::font= NULL;

void Canvas::Alloc_Color(GdkColor &color, RGB rgb)
{
	color.red=   rgb.r * 0x100;
	color.green= rgb.g * 0x100;
	color.blue=  rgb.b * 0x100;

	if (FALSE == gdk_colormap_alloc_color
	    (gdk_colormap_get_system(), &color, FALSE, TRUE))
		gdk_color_white(gdk_colormap_get_system(), &color);
}

void Canvas::Draw_Ellipse(GdkRectangle rectangle)
{
	assert (Is()); 

	gdk_draw_arc(drawable,
		     gc,
		     TRUE,
		     rectangle.x,
		     rectangle.y,
		     rectangle.width,
		     rectangle.height,
		     0,
		     64 * 360);
}

void Canvas::Draw_Canvas(const Canvas &canvas,
			 GdkRectangle dst,
			 GdkPoint src)
{
	assert (Is() && canvas.Is());

	gdk_draw_pixmap(drawable,
			gc,
			canvas.drawable,
			src.x, src.y,
			dst.x, dst.y,
			dst.width, dst.height);
}

void Canvas::
Draw_Stretch_Canvas(const Canvas &canvas,
		    GdkRectangle dst,
		    GdkRectangle src)
{
	Image::Draw_Stretch_Canvas(*this, canvas, dst, src); 
}

void Canvas::Draw_Label(GdkPoint point,
			/*String text*/
			const char *text,
			int text_length
			)
{
	assert (Is());

	if (! Load_Font())
		return;

	gdk_draw_text(drawable,
		      font,
		      gc,
		      point.x, point.y,
		      text,
		      text_length);
}

void Canvas::Set_Function(GdkFunction function)
{
	assert (Is());
	gdk_gc_set_function(gc, function);
}

bool Canvas::Load_Font() 
{
	if (font != NULL)  return true; 

	const char *font_name= "-*-helvetica-bold-r-normal--*-120-*-*-*-*-iso8859-1"; 

	font= gdk_font_load(font_name);

	if (font == NULL)
	{
		return false;
	}

	return true; 
}

int Canvas::Get_Label_Width(const char *text, int text_length)
{
	assert (text != NULL);

	if (! Load_Font())  return 1;

	gint lbearing, rbearing, width, ascent, descent;

	gdk_text_extents(font,
			 text,
			 text_length,
			 & lbearing,
			 & rbearing,
			 & width,
			 & ascent,
			 & descent);

	return width; 
}

