#include "widget.hh"

#include "input_reader.hh"
#include "gtk_timer.hh"

#include <gtk/gtkcontainer.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkbox.h>

int Widget::depth= -1; 

bool Widget::Open(Pointer <Widget> new_parent,
		  GtkWidget *new_gtkwidget,
		  bool dont_show)
{
	assert (! Is()); 

	if (new_gtkwidget == NULL)  return false;

	if (depth == -1)
	{
		GdkVisual *visual= gtk_widget_get_visual(new_gtkwidget);
		if (visual != NULL)
			depth= visual->depth; 
	}

	gtkwidget= new_gtkwidget;

	gtk_signal_connect(GTK_OBJECT(gtkwidget), "delete-event",
			   GTK_SIGNAL_FUNC(Signal_Delete), this);
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "destroy",
			   G_CALLBACK(Signal_Destroy), this);

	if (new_parent.Is())
	{
		parent= new_parent(); 

		if (! parent().Append(this))
		{
			Widget::Close();
			return false;
		}
		if (! dont_show)
			gtk_widget_show(new_gtkwidget); 

		parent().Wdg_Append_Child(*this); 
	}
	else
	{
		if (! dont_show)
			gtk_widget_show(new_gtkwidget);
	}

	return true;
}

bool Widget::Append(Pointer <Widget> child)
{
	if (! children.Set(*children + 1))  return false;
	children[-1]= child;
	return true; 
}

void Widget::Wdg_Close()
{
#ifndef NDEBUG
	wdg_closed= true;
#endif

	/* Children are all closed by GTK automatically.  Forget
	 * them. */
	children.Free();

	/* Remove self from parent child list */
	if (parent.Is())
	{
		for (int i= 0;  i < *parent().children;  ++i)
		{			
			if (parent().children[i] == this)
			{
				if (i + 1 < *parent().children)
					parent().children[i]= parent().children[-1];
				parent().children= -1; 
				goto found;
			}
		}
		/* The next line fails, but should not
		 */
#if 1-1
		assert (0); 
#endif /* 1-1 */ 
	found:
		parent= NULL; 
	}
	Message_Receiver::Remove_Pending_Messages();
}

Widget::Widget()
	:  gtkwidget (NULL)
{ 
}

void Widget::MR_Receive(String message)
{
	if (parent.Is())
	{
		parent->MR_Receive(message); 
		return;
	}

	if (message == "cancel")
		Close();

	else if (message == "ok")
		Close();
}

Widget::~Widget() 
{
	assert (gtkwidget == NULL); 
}

void Widget::Wdg_Append_Child(Widget &widget)
{
	gtk_container_add(GTK_CONTAINER(Get()), widget.Get()); 
}

void Widget::Set_Visible(bool visible)
{
	if (visible)
	{
		gtk_widget_show(Get());
	}
	else
	{
		gtk_widget_hide(Get());
	}
}

gint Widget::Signal_Delete(GtkWidget *this_gtkwidget,
			   GdkEvent  * /* event */ ,
			   gpointer   data)
{
	Widget &widget= *(Widget *)data;
	assert (widget.gtkwidget == this_gtkwidget);

	return ! widget.Wdg_Close_Query();
}

void Widget::Signal_Destroy(GtkWidget *,
			    gpointer data)
{
	Widget &widget= *((Widget *)data); 

#ifndef NDEBUG
	widget.wdg_closed= false;
#endif 
	
	widget.Wdg_Close(); 

#ifndef NDEBUG
	assert (widget.wdg_closed);
#endif

	widget.gtkwidget= NULL;
}

void Widget::Close(bool force)
{
	assert (gtkwidget != NULL);

	if (force || Wdg_Close_Query())
	{
		gtk_widget_destroy(gtkwidget);
		/* gtkwidget is set to NULL by Signal_Destroy().  */ 
		assert (gtkwidget == NULL);
	}
}

void Widget::Connect_Key_Signals(GtkWidget *widget)
{
	Verbose("widget::connect key signals"); 

	gtk_widget_add_events(widget, GDK_KEY_PRESS); 

	gtk_signal_connect(GTK_OBJECT(gtkwidget), "key-press-event",
			   GTK_SIGNAL_FUNC(Signal_Key_Press_Event), this);
}

gint Widget::Signal_Key_Press_Event(GtkWidget *,
				    GdkEventKey *e,
				    gpointer data)
{
	Verbose("widget::signal key press"); 

	Widget &widget= *(Widget *)data;

	return widget.Wdg_Key_Press(e->keyval);
}

void Widget::Set_Index(int index)
{
	gtk_box_reorder_child(GTK_BOX(parent->Get()), Get(), index);
}

 /* Input reader */ 
static void Widget_Input_Function(gpointer data,
				  gint /* source */ ,
				  GdkInputCondition gic)
{
	Verbose_Function(); 

	if (! (gic & GDK_INPUT_READ))   return; 

	Input_Reader &ir= *(Input_Reader *)data;
  
	ir.IR_Input();
}

int Register_Input(Input_Reader &ir)
{
	guint id= gtk_input_add_full
		(ir.FD(),
		 GDK_INPUT_READ,
		 & Widget_Input_Function,
		 NULL,
		 & ir,
		 NULL);
	
	Verbose("Register_Input() = %d", id); 

	return id; 
}

void Unregister_Input(Input_Reader &ir)
{
	gtk_input_remove(ir.ID()); 
}
