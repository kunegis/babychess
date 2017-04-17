#ifndef LIST_HH
#define LIST_HH

#include "scrolled.hh"

#include <gtk/gtkliststore.h>

class List
	:  public Widget
{
public:
	struct Open_Info
	{
		int column_count;
		Scroll_Direction scroll_direction;
		String msg_clicked;
		const char *const *const titles;
		bool fill;

		Open_Info(int new_column_count,
			  Scroll_Direction new_scroll_direction,
			  String new_msg_clicked,
			  const char *const *const new_titles,
			  bool new_fill= false)
			: column_count(new_column_count),
			  scroll_direction(new_scroll_direction),
			  msg_clicked(new_msg_clicked),
			  titles(new_titles),
			  fill(new_fill)
			{}

	};

	bool Open(Pointer <Widget> parent,
		  Open_Info open_info);
	void Clear();
	void Append_Row();
	void Set(int column, int row, const char *text);

private:
	GtkListStore *store;	
};

#endif /* ! LIST_HH */ 
