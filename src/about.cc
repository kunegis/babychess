#include "about.hh"

#include "install_dir.hh"
#include "version_number.hh"
#include "icon.hh"
#include "format.hh"
#include "line_reader.hh"

#include <vector>
#include <gtk/gtkaboutdialog.h>

GdkPixbuf *New_About_Image()
{
	static const char *name= NULL;
	if (name == NULL)
	{
		name= g_strdup_printf("%s/share/about/title.png",
				      Install_Dir()());
		if (name == NULL)
			return NULL;
	}
	static GdkPixbuf *image= NULL;
	if (image == NULL)
		image= gdk_pixbuf_new_from_file(name, NULL);
	return image; 
}


void New_About()
{
	GtkAboutDialog *dialog= GTK_ABOUT_DIALOG(gtk_about_dialog_new());
	if (dialog == NULL)  return;

	Set_Icon(GTK_WINDOW(dialog));
	gtk_about_dialog_set_name(dialog, "BabyChess");
	gtk_about_dialog_set_version(dialog, VERSION_NUMBER);
	gtk_about_dialog_set_copyright(dialog, COPYRIGHT_LINE);
	gtk_about_dialog_set_website(dialog, WEBSITE);
	gtk_about_dialog_set_translator_credits
		(dialog, 
		 _("translator-credits"));
	gtk_about_dialog_set_logo(dialog, New_About_Image());

	String comments= Format(_("A chess application\nfrom %s"), Version_Date());
#ifndef NDEBUG
	comments += '\n';
	comments += _("Debug version (NDEBUG not defined)");
#endif /* ! NDEBUG */
	comments += Format("\nCXXFLAGS = %s\n", CXXFLAGS);
	gtk_about_dialog_set_comments(dialog, comments());
	
	static std::vector <const char *> authors;
	if (authors.size() == 0)
	{
		String name= Install_Dir() + "/AUTHORS";
		FILE *file= fopen(name(), "r");
		if (file != NULL)
		{
			Line_Reader line_reader(file);
			while (line_reader.Read())
			{
				authors.push_back(strdup(line_reader.Line())); 
			}
			fclose(file);
		}
		authors.push_back(NULL);
	}
	gtk_about_dialog_set_authors(dialog, &*authors.begin());
	
	gtk_widget_show(GTK_WIDGET(dialog));
}
