#include "toolbar.hh"

#include "verbose.hh"
#include "install_dir.hh"
#include "filename.hh"

#include <gtk/gtktoolbar.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkbutton.h>

GtkWidget *Toolbar::Append(const char *stock_name, 
			   String msg, 
			   const char *tooltip)
{
	return gtk_toolbar_insert_stock
		(GTK_TOOLBAR(Get()),
		 stock_name,  
		 tooltip,	
		 "private-help",
		 (GtkSignalFunc) & Callback::Call,
		 New_Callback(msg),
		 -1);
}

GtkWidget *Toolbar::Append_Text(const char *text, 
				String msg, 
				const char *tooltip)
{
	return gtk_toolbar_insert_item
		(GTK_TOOLBAR(Get()),
		 text,
		 tooltip,
		 "private-help", /* what is that argument for ? */ 
		 NULL,
		 (GtkSignalFunc) & Callback::Call,
		 New_Callback(msg),
		 -1);
}

gint Toolbar::Callback::Call(GtkWidget *,
			      gpointer data)
{
	if (data == NULL)  return  TRUE; 
	
	Callback &callback= *(Callback *)data;
	callback.toolbar().MR_Receive(callback.msg);

	return TRUE;
}

Toolbar::Callback *Toolbar::New_Callback(String msg)
{
	if (! callbacks.Set(*callbacks + 1))
		return NULL; 

	if (! callbacks[-1].Alloc(this, msg))
	{
		callbacks= -1;
		return NULL;
	}
	return & callbacks[-1]();
}

GtkWidget *Toolbar::Append_Image(const char * /* text */ , 
				 String message, 
				 const char *tooltip,
				 const char *image_name)
{
	String name= Install_Dir() / "share/toolbar" / image_name + ".png"; 
	GtkWidget *image= gtk_image_new_from_file(name());
	if (!image)  return NULL;
	GtkWidget *button= gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button), image);
	gtk_widget_show(image);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_widget_show(button); 
	gtk_toolbar_append_widget(GTK_TOOLBAR(Get()),
				  button,
				  tooltip,
				  "private-help");
	gtk_signal_connect(GTK_OBJECT(button), 
			   "clicked",
			   (GtkSignalFunc) & Callback::Call,
			   New_Callback(message));
	return button; 
}

void Toolbar::Wdg_Append_Child(Widget &widget)
{
	gtk_toolbar_append_widget(GTK_TOOLBAR(Get()),
				  widget.Get(),
				  next_tooltip ?: "",
				  "private-help");
	next_tooltip= NULL;
}

bool Toolbar::Open(Pointer <Widget> parent)
{
	next_tooltip= NULL;
	if (! Widget::Open(parent, gtk_toolbar_new()))
		return false;
	gtk_toolbar_set_style(GTK_TOOLBAR(Get()), GTK_TOOLBAR_ICONS);
//	gtk_toolbar_set_show_arrow(GTK_TOOLBAR(Get()), TRUE);
	return true;
}

void Toolbar::Append_Space()
{
	gtk_toolbar_append_space(GTK_TOOLBAR(Get())); 
}
