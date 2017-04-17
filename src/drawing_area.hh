#ifndef DRAWING_AREA_HH
#define DRAWING_AREA_HH

#include "widget.hh"
#include "canvas.hh"

class Drawing_Area
	:  public Widget
{
public:
	bool Open(Pointer <Widget> parent); 

	void Invalidate()
		{
			gtk_widget_queue_draw(Get());
		}

	void Invalidate(const GdkRectangle &rectangle)
		{
			gtk_widget_queue_draw_area
				(Get(),
				 rectangle.x,
				 rectangle.y,
				 rectangle.width,
				 rectangle.height); 
		}
	
	GdkPoint Size() const
		{
			assert (Is());

			GdkPoint result= {Get()->allocation.width,
					  Get()->allocation.height};

			return result;
		}
	virtual void Wdg_Draw(Canvas &) { }
	virtual void Wdg_Size(GdkPoint)  { }
	/* To do:  check return values of signals handlers  */

private:
	static void Signal_Expose(GtkWidget *this_gtkwidget,
				  GdkEventExpose *event,
				  gpointer data); 
	static void Signal_Size_Allocate(GtkWidget *,
					 GtkAllocation *,
					 gpointer);
	static void Signal_Size_Request(GtkWidget *,
					GtkRequisition *,
					gpointer);
};

#endif /* ! DRAWING_AREA_HH */ 
