#ifndef LIST_HH
#define LIST_HH

#include "widget.hh"
#include "scrolled.hh"
#include "framed.hh"

#include <gtk/gtkclist.h>

class List_Content
	:  public Widget
{
public:

	struct Open_Info
	{
		int column_count;
		String msg_clicked;
		Pointer <const Vector <String> > titles;
	}; 

	bool Open(Pointer <Widget> parent,
		  int new_column_count= 2,
		  String msg_clicked= "",
		  Pointer <const Vector <String> > titles= NULL);
	bool Open(Pointer <Widget> parent,
		  Open_Info open_info)
		{
			return Open(parent,
				    open_info.column_count,
				    open_info.msg_clicked,
				    open_info.titles);
		}
	
	void Clicked(int &r, int &c)
		{
			r= row;
			c= column;
		}
	void Set_Selection(int new_row);

	int Selection() const
		{ return row; }

	void Set(int column, int row, String text);
	String Text(int column, int row)
		{
			const char *text;
			if (!gtk_clist_get_text(GTK_CLIST(Get()), row, column, (gchar**)&text))
				return "";
			return String(text);
		}

	void Set_Row_Count(int rows);
	int Row_Count() const
		{ return row_count;  }
	void Set_Title(int col, String title)
		{
			gtk_clist_set_column_title
				(GTK_CLIST(Get()),
				 col,
				 title());
		}

	bool Double_Click() const
		{ return double_click;   }
	void Set_Column_Right_Aligned(int column)
		{
			gtk_clist_set_column_justification(GTK_CLIST(Get()),
							   column,
							   GTK_JUSTIFY_RIGHT); 
		}
	void Remove(int rrow)
		{
			Freeze();
			gtk_clist_remove(GTK_CLIST(Get()), rrow);
			--row_count;
			if (row >= row_count)  row= -1;
		}
	void Flush()
		{
			if (! frozen)  return;
			MR_Receive("thaw");
		}

private:
	
	int row_count;
	int column_count;
	/* Set in Open(), never changed. */
	int row, column;
	/* Set by Signal_Select_Row().  Returned by Clicked().  */
	String msg_clicked; 
	bool double_click;
	int optimal_width;
	bool frozen;
	bool in_set;

	void Wdg_Get_Size_Request(int &width,
				  int &height,
				  bool &resizeable);

	void MR_Receive(String);
	void Freeze();
	static void Signal_Select_Row (GtkCList *clist,
				       gint row,
				       gint column,
				       GdkEventButton *event,
				       gpointer user_data);
};

class List
	:  public Framed <Scrolled <List_Content> >
{
public:

	struct Open_Info
	{
		List_Content::Open_Info content_open_info;
		Scroll_Direction scroll_direction;
		bool fill;

		Open_Info(int column_count,
			  Scroll_Direction new_scroll_direction,
			  String msg_clicked,
			  Pointer <const Vector <String> > titles= NULL,
			  bool new_fill= false)
			: scroll_direction(new_scroll_direction),
			  fill(new_fill)
			{
				List_Content::Open_Info oi=
					{column_count, msg_clicked, titles};
				content_open_info= oi; 
			}
	};

	bool Open(Pointer <Widget> parent,
		  Open_Info open_info);

	void Set_Selection(int row, bool scroll_to_visible= false);

	List_Content *operator -> () 
		{return & Containing <Frame, Scrolled <List_Content> >::operator()()(); }
};

#endif /* ! LIST_HH */ 
