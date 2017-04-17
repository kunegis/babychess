#include "start_window.hh"

#include "gui_execute.hh"
#include "format.hh"
#include "install_dir.hh"
#include "filename.hh"
#include "about.hh"
#include "gettext.hh"

#include <gtk/gtkimage.h>
#include <gtk/gtkstock.h>
#include <gtk/gtkbox.h>

Start_Window::Info Start_Window::info[Start_Window::size]=
{{N_("_Play"),                   "--play"},
 {N_("Play on the _internet"),   "--internet"},
 {N_("New game _archive"),       "--edit-game-archive"},
 {N_("New _game"),               "--edit-game"},
 {N_("New _position"),           "--edit-position"}};

bool Start_Window::Open(Open_Info)
{
	if (! Window::Open("BabyChess", (GdkPoint){-1, -1}))  return false;

	H_Box::Open_Info oi(dialog_unit, false, false);
	if (! main_box.Open(this, oi))  goto close;

	gtk_container_set_border_width(GTK_CONTAINER(Get()), dialog_unit); 

	{
		GtkWidget *image= gtk_image_new_from_pixbuf(New_About_Image());
		if (image == NULL)  
		{
			Verbose("\t image == NULL");
			goto close;
		}
		Verbose_Pointer(image);

		gtk_box_pack_start_defaults(GTK_BOX(main_box.Get()),
					    GTK_WIDGET(image));
		gtk_widget_show(GTK_WIDGET(image));
	}

	if (! button_box.Open(main_box, dialog_unit/2))  goto close;

	for (int i= 0;  i < size;  ++i)
	{
		if (i == 2)
			first_bevel.Open(button_box);
		if (! buttons[i].Open(button_box, Format("start %d", i), _(info[i].text)))
		{
			goto close; 
		}
	}

	bevel.Open(button_box); 
	about_button.Open(button_box, "about", _("_About")); 
	close_button.Open(button_box, "ok", "", true, GTK_STOCK_CLOSE); 

	return true;

 close:
	Window::Close(); 
	return false;
}

void Start_Window::MR_Receive(String message)
{
	if (!memcmp(message(), "start ", 6))
	{
		int i;
		if (1!=sscanf(message()+6, "%d", &i))  return;
		if (i < 0 || i >= size)  return;
		_GUI_Execute(this, info[i].mode);
		Window::MR_Receive("ok"); 
	}
	else if (message == "about")
	{
		New_About(); 
	}
	else
		Window::MR_Receive(message);
}
