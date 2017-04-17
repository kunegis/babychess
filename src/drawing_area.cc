#include "drawing_area.hh"

#include <gtk/gtkdrawingarea.h>
#include <gtk/gtksignal.h>

bool Drawing_Area::Open(Pointer <Widget> parent) 
{
	GtkWidget *gtkwidget= gtk_drawing_area_new(); 

	if (gtkwidget == NULL) return false;

	gtk_signal_connect(GTK_OBJECT(gtkwidget), "expose-event",
			   GTK_SIGNAL_FUNC(Signal_Expose), this); 
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "size-allocate",
			   GTK_SIGNAL_FUNC(Signal_Size_Allocate), this); 
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "size-request",
			   GTK_SIGNAL_FUNC(Signal_Size_Request), this); 

	if (!  Widget::Open(parent, gtkwidget))
		return false;

	int w= -1, h= -1;
	bool resizeable= false; 
	Wdg_Get_Size_Request(w, h, resizeable); 
	assert (w < 65 * dialog_unit);
	assert (h < 65 * dialog_unit);
  
	if (w >= 0 && h >= 0 && ! resizeable)
		gtk_drawing_area_size(GTK_DRAWING_AREA(gtkwidget), w, h);
  
	return true;
}

void Drawing_Area::Signal_Size_Allocate(GtkWidget *this_gtkwidget,
				  GtkAllocation *allocation,
				  gpointer data)
{
	Drawing_Area &drawing_area= *(Drawing_Area *)data;
	assert (drawing_area.Get() == this_gtkwidget);

	drawing_area.Wdg_Size((GdkPoint) {allocation->width, allocation->height}); 
}

void Drawing_Area::Signal_Size_Request(GtkWidget *this_gtkwidget,
				 GtkRequisition *requisition,
				 gpointer data)
{
	Drawing_Area &drawing_area= *(Drawing_Area *)data;
	assert (drawing_area.Get() == this_gtkwidget);

	int w= requisition->width;
	int h= requisition->height;
	assert (w  >= -1);
	assert (h  >= -1);
	bool resizeable= false;
	drawing_area.Wdg_Get_Size_Request(w, h, resizeable);
	if (w <= 0) w= 1;
	if (h <= 0) h= 1;
	requisition->width= w;
	requisition->height= h; 
}

void Drawing_Area::Signal_Expose(GtkWidget *this_gtkwidget,
			   GdkEventExpose *,
			   gpointer data)
{
	Drawing_Area &drawing_area= *(Drawing_Area *)data;
	assert (drawing_area.Get() == this_gtkwidget);

	Canvas canvas(this_gtkwidget->window,
		      this_gtkwidget->style->fg_gc[GTK_WIDGET_STATE(this_gtkwidget)]); 
	drawing_area.Wdg_Draw(canvas); 

	canvas.Set_Function(GDK_COPY);
	GdkColor black;
	gdk_color_black(gdk_colormap_get_system(), &black);
	gdk_gc_set_foreground(canvas.Get_GC(), &black);
}
