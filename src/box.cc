#include "box.hh"

#include "verbose.hh"

#include <gtk/gtkvbox.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtktable.h>

void Box::Wdg_Get_Size_Request(int &/*width*/,
				  int &/*height*/,
				  bool &resizeable)
{
	resizeable= false;
}

bool Box::Open(Pointer <Widget> parent,
		  Open_Info open_info)
{
	GtkWidget *gtkwidget= Box_Open(open_info.spacing, open_info.homogeneous);
			
	if (gtkwidget == NULL)
		return false;

	gtk_widget_set_usize(gtkwidget, -1, -1);

	if (! Widget::Open(parent, gtkwidget))
		return false;

	use_next= false;
	reverse= open_info.reverse;
			
	return true;
}

void Box::Wdg_Append_Child(Widget &widget)
{
	if (!use_next)
	{
		next_resizeable= false;
		int w= -1, h= -1;
		widget.Wdg_Get_Size_Request(w, h, next_resizeable); 
		next_padding= 0;
	}
	else
		use_next= false;

	gboolean arg= next_resizeable ? TRUE : FALSE;

	if (!reverse)
		gtk_box_pack_start(GTK_BOX(Get()), widget.Get(), 
				   arg, arg, next_padding);
	else 
		gtk_box_pack_end  (GTK_BOX(Get()), widget.Get(), 
				   arg, arg, next_padding);
}

GtkWidget *V_Box::Box_Open(int spacing, bool homogeneous) const
{
	return gtk_vbox_new(homogeneous, spacing);
}

GtkWidget *H_Box::Box_Open(int spacing, bool homogeneous) const
{
	return gtk_hbox_new(homogeneous ? TRUE : FALSE, spacing);
}

void Table::Wdg_Append_Child(Widget &widget)
{
	/* widget is included in
	 * Get_Children_Count().
	 */
	int children= Get_Children_Count() - 1 + space;
	
	if (fill)
		gtk_table_attach
		(GTK_TABLE(Get()),
		 widget.Get(),
		 children % columns,
		 children % columns + width,
		 children / columns,
		 children / columns + height,
		 GTK_FILL, GTK_FILL, 0, 0);
	else
		gtk_table_attach_defaults
		(GTK_TABLE(Get()),
		 widget.Get(),
		 children % columns,
		 children % columns + width,
		 children / columns,
		 children / columns + height);
	width= height= 1;
	fill= false;
}

bool Table::Open(Pointer <Widget> parent,
		 Open_Info open_info)
{
	columns= open_info;
	space= 0;
	width= height= 1;
	fill= false;
	return Widget::Open(parent, gtk_table_new(1, open_info, TRUE));
}
