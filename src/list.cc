#include "list.hh"

#include "verbose.hh"

#include <gtk/gtkscrolledwindow.h>

#include <algorithm>

bool List_Content::Open(Pointer <Widget> parent,
			int new_column_count,
			String new_msg_clicked,
			Pointer <const Vector <String> > titles)
{
	column_count= new_column_count; 

	row= -1;
	row_count= 0; 
	in_set= false;

	const gchar *text[column_count];
	for (int i= 0;  i < column_count;  ++i)
		text[i]= titles.Is() 
			?(gchar*)titles()[i]()
			: "";
	GtkWidget *gtkwidget= gtk_clist_new_with_titles
		(column_count,
		 (gchar **) text); 

	if (gtkwidget == NULL)
		return false;
	
	optimal_width= 0;
	for (int i= 0;  i < column_count;  ++i)
	{
		optimal_width += std::max(gtk_clist_optimal_column_width(GTK_CLIST(gtkwidget), i), 30);
		gtk_clist_set_column_auto_resize 
			(GTK_CLIST(gtkwidget),
			 i,
			 TRUE);
	}

	if (!titles)
		gtk_clist_column_titles_hide(GTK_CLIST(gtkwidget)); 

	if (! Widget::Open(parent, gtkwidget))
		return false;

	msg_clicked= new_msg_clicked; 
	frozen= false;

	gtk_signal_connect(GTK_OBJECT(Get()),
			   "select-row",
			   GTK_SIGNAL_FUNC(& Signal_Select_Row),
			   this);

	return true;
}

void List_Content::
Signal_Select_Row(GtkCList * /* clist */ ,
		  gint new_row,
		  gint new_column,
		  GdkEventButton *event,
		  gpointer data)
{
	Verbose("List:: select (row = %d, column = %d, event = %p)",
	    new_row, new_column, event);

	List_Content &box= *(List_Content *)data;

	if (box.in_set)  return; 

	box.row= new_row;
	box.column= new_column; 

	box.double_click= event && event->type == GDK_2BUTTON_PRESS; 
	box.MR_Receive(box.msg_clicked);
}

void List_Content::Set_Row_Count(int new_row_count)
{
	Freeze();
	if (new_row_count == 0)
	{
		gtk_clist_clear(GTK_CLIST(Get()));
	}
	else
	{
		if (new_row_count > row_count)
			/* add rows */
		{
			gchar *text[column_count];
			for (int i= 0;  i < column_count;  ++i)
				text[i]= "";
			gtk_clist_freeze(GTK_CLIST(Get()));
			for (int i= row_count;  i < new_row_count;  ++i)
			{
				gtk_clist_append(GTK_CLIST(Get()),
						 text); 
			}
			gtk_clist_thaw(GTK_CLIST(Get()));
		}
		else if (new_row_count < row_count)
			/* remove rows */ 
		{
			gtk_clist_freeze(GTK_CLIST(Get()));
			for (int i= row_count - 1;  i >= new_row_count;  --i)
				gtk_clist_remove(GTK_CLIST(Get()),
						 i);
			gtk_clist_thaw(GTK_CLIST(Get()));
			if (row >= new_row_count)
				row= -1;
		}
	}

	row_count= new_row_count;
}

void List_Content::Set_Selection(int new_row)
{
	row= new_row;
	in_set= true;
	gtk_clist_select_row(GTK_CLIST(Get()),
			     row,
			     0);
	in_set= false;
}

void List::Set_Selection(int row, bool scroll)
{
	Verbose("Set_Selection(row= %d, scroll= %d)", row, scroll);

	(*this)->Set_Selection(row); 

	if (! scroll)
		return; 

	/* If the selected cell is not visible, scroll to make it
	 * visible. */

	(*this)->Flush();
	GtkAdjustment *a= gtk_scrolled_window_get_vadjustment
		(GTK_SCROLLED_WINDOW((*this)().Get()));

	if (a == NULL)
		return;

	/* what is currently visible */ 
	float min_visible= a->value;
	float max_visible= a->value + a->page_size;

	/* coordinates of the cell */ 
	float new_ideal_value_min=
		a->lower 
		+ ((a->upper - a->lower) 
		   * row 
		   / (*this)->Row_Count());
	float new_ideal_value_max=
		a->lower 
		+ ((a->upper - a->lower) 
		   * (row + 1)
		   / (*this)->Row_Count());
	Verbose("\tideal= %f ... %f", 
	    new_ideal_value_min,
	    new_ideal_value_max);

	/* cell is already fully visible */ 
	if (new_ideal_value_min >= min_visible &&
	    new_ideal_value_max <= max_visible)
		return;

	/* new min value so the cell is centered */ 
	float new_min= 
		(new_ideal_value_min + (a->upper - a->lower) 
		 / (*this)->Row_Count() / 2)
		- a->page_size / 2;

	/* would scroll before the beginning */
	if (new_min < a->lower)  
		new_min= a->lower;
	
	/* would scroll after the end */
	if (new_min + a->page_size > a->upper)
		new_min= a->upper - a->page_size;
	

	gtk_adjustment_set_value
		(GTK_ADJUSTMENT(a),
		 new_min);

	gtk_adjustment_value_changed(a);
}

bool List::Open(Pointer <Widget> parent,
		Open_Info open_info)
{
	return Framed <Scrolled <List_Content> > ::Open
		(parent, Scrolled  <List_Content>
		 ::Open_Info(Scroll_Widget::Open_Info
			     (open_info.scroll_direction), 
			     open_info.content_open_info));
}

void List_Content::Wdg_Get_Size_Request(int &width,
					int &height,
					bool &r_resizeable)
{
	width= optimal_width; 
	height= 16 * dialog_unit*5/3; 
	r_resizeable= true;
}

void List_Content::Freeze()
{
	if (frozen)  return;

	frozen= true;
	gtk_clist_freeze(GTK_CLIST(Get()));
	MR_Send("thaw");
}

void List_Content::MR_Receive(String message)
{
	if (message == "thaw")
	{
		if (!frozen)  return; 
		gtk_clist_thaw(GTK_CLIST(Get()));
		frozen= false; 
	}
	else
		Widget::MR_Receive(message);
}

void List_Content::Set(int column, int row, String text)
{
	Verbose("List::Set(col= %d, row= %d, \"%s\")",
	    column, row, text());
	Freeze();
	gtk_clist_set_text(GTK_CLIST(Get()),
			   row, column, text());
}
