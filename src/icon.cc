#include "icon.hh"

#include "install_dir.hh"
#include "filename.hh"

void Set_Icon(GtkWindow *window)
{
	static GdkPixbuf *icon= NULL;

	if (icon == NULL)
	{
		String filename= Install_Dir() / "babychess.png";
		GError *error= NULL;
		icon= gdk_pixbuf_new_from_file(filename(), &error);
	}

	if (icon != NULL)
	{
		gtk_window_set_icon(window,  icon);
	}
}
