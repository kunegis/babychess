#ifndef WIDGET_HH
#define WIDGET_HH

/* This module defines the functions needed by INPUT_READER. 
 */

#include "vector.hh"
#include "message_receiver.hh"

#include <gtk/gtkwidget.h>

class Widget
	:  virtual public Message_Receiver
{
public:
	enum {dialog_unit= 12}; /* as read in the HIG */ 

	Widget();

	virtual ~Widget();

	bool Open(Pointer <Widget> parent,
		  GtkWidget *gtkwidget,
		  bool dont_show = false);
	/* GTKWIDGET as returned by a GTK function.
	 * GTKWIDGET may be NULL. */

	bool Append(Pointer <Widget> child); 

	void MR_Receive(String);

	int Get_Children_Count() const
		{  return *children;   }

	Widget &Root()
		{
			if (parent.Is())
				return parent->Root();
			else
				return *this; 
		}
	const Widget &Root() const
		{
			if (parent.Is())
				return parent->Root();
			else
				return *this; 
		}

	Widget &Get_Child(int n) const
		{ return children[n]();   }

	void Set_Visible(bool visible= true);

	void Parent_Send_Message(String message)
		{
			if (parent.Is())
				parent->MR_Send(message);
		}

	void Parent_Receive_Message(String message)
		{
			parent->MR_Receive(message);
		}
	

	void Set_Index(int index);

	void Close(bool force= false); 
	bool Is() const
		{ return gtkwidget != NULL;  }

	GtkWidget *Get() const
		{ return gtkwidget;  }
	void Connect_Key_Signals(GtkWidget *gtkwidget);
	/* To be called after opening when these signals are used. 
	 */

	virtual void Wdg_Get_Size_Request(int & /* width */ ,
					  int & /* height */ ,
					  bool & /* resizeable */ )
		/* Return preferred size, and whether widget can be
		 * resized.  The widget may check width and height.
		 * If one of them is not -1, then the returned value
		 * should not be bigger than that value. 
		 */
		{ }

	virtual bool Wdg_Key_Press(guint)
		/* Return value:  whether the key was processed. 
		 */
		{  return false;  }
	virtual bool Wdg_Close_Query()
		/* Whether the widget can be closed.  Called before
		 * Wdg_Close().  */
		{   return true;   }
	virtual void Wdg_Close(); 
	virtual void Wdg_Append_Child(Widget &);
	/* Widgets that can have children (as V_Box) implement this to
	 * call e.g. gtk_box_pack_start(). 
	 */

	static int Get_Depth()
		{
			assert (depth > 0);
			return depth; 
		}

private:
	GtkWidget *gtkwidget;
	Vector <Pointer <Widget> > children;
	Pointer <Widget> parent;

#ifndef NDEBUG
	bool wdg_closed;
#endif 

	static int depth;
	static gint Signal_Delete(GtkWidget *this_gtkwidget,
				  GdkEvent  * /* event */ ,
				  gpointer   data); 
	/* Called before destruction of an object.  Only
	 * called for windows.    */

	static void Signal_Destroy(GtkWidget *,  gpointer   data); 
	/* Called after destruction  */

	static gint Signal_Key_Press_Event(GtkWidget *,
					   GdkEventKey *,
					   gpointer);
};

#endif /* ! WIDGET_HH */
