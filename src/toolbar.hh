#ifndef TOOLBAR_HH
#define TOOLBAR_HH

#include "widget.hh" 
#include "dynamic.hh"

/* Note:  no access keys like "_Open" in toolbar text. 
 */

class Toolbar
	:  public Widget
{
public: 
	const char *next_tooltip; 

	bool Open(Pointer <Widget> parent);

	/* stock item */ 
	GtkWidget *Append(const char *stock_name,
			  String message,
			  const char *tooltip= "");

	/* text only */ 
	GtkWidget *Append_Text(const char *text, 
			       String message, 
			       const char *tooltip);
	/* image */ 
	GtkWidget *Append_Image(const char *text, 
				String message, 
				const char *tooltip,
				const char *image_name);

	void Append_Space();

private:

	class Callback
	{
	public:

		Pointer <Toolbar> toolbar;
		String msg;

		Callback(Pointer <Toolbar> new_toolbar,
			 String new_msg)
			:  toolbar(new_toolbar),
			   msg(new_msg)
			{ }

		static gint Call(GtkWidget *, gpointer data);
	};

	Vector <Dynamic <Callback> > callbacks;

	void Wdg_Append_Child(Widget &widget);

	Callback *New_Callback(String msg);
};

#endif /* ! TOOLBAR_HH */ 
