#include "document_editor.hh"

#include "verbose.hh"
//#include "ask.hh"
#include "format.hh"
#include "gui_execute.hh"
#include "error.hh"
#include "gettext.hh"
#include "inform.hh"

#include "../dep/strings.hh"

#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>

#include <gtk/gtkstock.h>

#define s_asked "asked"

void Document_Editor_Base::Load()
{
	if (modified >= 2)
	{
		asked_for_what= 1;
		Ask(as_load);
	}
	else
	{
		if (Owner() == NULL)
			Ask_Load_Name();
		else
			DO_Save(""); 
	}
}

bool Document_Editor_Base::Save()
{
	if (Has_Filename())
		return Do_Save();
	else
	{
		Save_As();
		return false; 
	}
}

void Document_Editor_Base::Save_As()
{
	asked_save_for= 1;
	Ask_Save_Name(filename);
}

void Document_Editor_Base::Save_Other()
{
	asked_save_for= 4;
	Ask_Save_Name(filename);
}

bool Document_Editor_Base::Do_Save()
{
	assert (filename.Is());
	if (DO_Save(filename))
	{
		modified= 1;
		DO_Modified_Changed(modified);
		return true;
	}
	else
	{
		if (Owner() == NULL)
			Save_Error(); 
		return false; 
	}
}

bool Document_Editor_Base::Asked_Load_Filename(String new_filename)
{
	assert (new_filename.Is());

	return Load_File(new_filename);
}

bool Document_Editor_Base::Load_File(String new_filename,
				     bool anonymous)
{
	Verbose_Function(); 

	assert (new_filename.Is());

	switch (DO_Load(new_filename))
	{
	default:
		assert(0);
		return false;

	case lr_error:
	{
		String message= Format(_("Cannot open \"%s\""), new_filename());
		if (Widget::Is())
		{
			Close_Dialogs();
			Inform_Error(message, this);
		}
		else
		{
			/* Save ERRNO because opening a window
			 * may lose ERRNO.  Also don't pass THIS as second
			 * argument, because the window is closed. 
			 */
			int e= errno;
			Inform_Error(message, NULL);
			errno= e;
		}
		return false;
	}

	case lr_success:
		if (!anonymous && Owner() == NULL)
		{
			filename= new_filename;
			Filename_Changed(filename);
		}
		/* fall through */
	case lr_wrong_type:
		return true;
	}
}

void Document_Editor_Base::Open(String new_filename)
{
	assert (Owner() == NULL);
	filename= new_filename;
	Filename_Changed(filename);
}

void Document_Editor_Base::Clear(bool ask)
{
	if (modified == 0 || !ask)
	{
		DE_Reset();
		if (Owner())
			modified= 2;
		else
			modified= 0;
		DO_Modified_Changed(modified); 
	}
	else
	{
		asked_for_what= 2;
		Ask(as_clear);
	}
}

void Document_Editor_Base::Modify()
{
	if (modified >= 2) return;
	modified= 2; 
	DO_Modified_Changed(modified);
}

void Document_Editor_Base::Close_Dialogs()
{
	ask_dialog.Close(); 
	file_dialog.Close();
}

void Document_Editor_Base::Filename_Changed(String filename)
{
	if (! Widget::Is())
		return;

	if (! filename.Is())
		filename= EW_Type_Name();
	else
	{
		filename.Isolate();
		String new_name= basename(filename.c()); 
		new_name.Isolate(); 
		filename= new_name; 
	}
	Editor_Base::Set_Caption(filename);
}

void Document_Editor_Base::Ask_Load_Name()
{
	Close_Dialogs(); 
	filename_for= 1;
	file_dialog.Open(File_Dialog::t_open,
			 "asked_filename",
			 *this,
			 Filename());
}

void Document_Editor_Base::Ask_Save_Name(String initial_name)
{
	Close_Dialogs();
	filename_for= 2;
	file_dialog.Open(File_Dialog::t_save,  
			 "asked_filename",
			 *this,
			 initial_name);
}

void Document_Editor_Base::Append_Menu(Menubar &menubar)
{
	static GtkItemFactoryEntry owned_entries[]=
		{{"/File/"N_("_New"),   NULL,
		  MENU_CALLBACK, str_new,"<StockItem>",GTK_STOCK_NEW},
		 {"/File/"N_("_Open"),NULL,
		  MENU_CALLBACK,str_open,"<StockItem>",GTK_STOCK_OPEN},
		 {"/File/sep1",       NULL,
		  NULL,         0,            "<Separator>",0},
		 {"/File/"N_("_Clear"),NULL,
		  MENU_CALLBACK,str_clear,"<StockItem>",GTK_STOCK_CLEAR},
		 { "/File/"N_("_Save"),         NULL, 
		   MENU_CALLBACK, str_save,     "<StockItem>",  GTK_STOCK_SAVE}, 
		 {"/File/sep2",       NULL,
		  NULL,         0,            "<Separator>",0},
		 {"/File/"N_("_Export"), NULL, MENU_CALLBACK,
		  str_export, NULL, 0},
		 {"/File/sep3",       NULL,
		  NULL,         0,            "<Separator>",0},
		 {"/File/"N_("Proper_ties"), "<alt>Return",
		  MENU_CALLBACK,  str_properties, "<StockItem>",GTK_STOCK_PROPERTIES},
		 {"/File/sep4",       NULL,
		  NULL,         0,            "<Separator>",0},
		 {"/File/"N_("_OK"),    NULL,
		  MENU_CALLBACK,str_ok,    "<StockItem>",GTK_STOCK_OK},
		 {"/File/"N_("_Apply"), NULL,
		  MENU_CALLBACK,str_apply, "<StockItem>",GTK_STOCK_APPLY},
		 {"/File/"N_("_Cancel"),NULL,
		  MENU_CALLBACK,str_cancel,"<StockItem>",GTK_STOCK_CANCEL}
		};

	static GtkItemFactoryEntry unowned_entries[]=
		{{ "/File/"N_("_New"),          NULL, 
		   MENU_CALLBACK, str_new,      "<StockItem>",  GTK_STOCK_NEW},  
		 { "/File/"N_("_Open"),         NULL, 
		   MENU_CALLBACK, str_open,     "<StockItem>",  GTK_STOCK_OPEN}, 
		 { "/File/sep1",                        NULL, 
		   NULL,          0,               "<Separator>",  0},
		 { "/File/"N_("_Clear"),        NULL, 
		   MENU_CALLBACK, str_clear,    "<StockItem>",  GTK_STOCK_CLEAR},
		 { "/File/"N_("_Load"),         NULL, 
		   MENU_CALLBACK, str_load,     NULL,           0},
		 { "/File/"N_("_Save"),         NULL, 
		   MENU_CALLBACK, str_save,     "<StockItem>",  GTK_STOCK_SAVE}, 
		 { "/File/"N_("Save _As..."),   NULL, 
		   MENU_CALLBACK, str_save_as,  "<StockItem>",  GTK_STOCK_SAVE_AS},
		 { "/File/sep2",                        NULL, 
		   NULL,          0,               "<Separator>",  0},
		 {"/File/"N_("_Export"), NULL, MENU_CALLBACK,
		  str_export, NULL, 0},
		 { "/File/sep3",       NULL,
		   NULL,         0,            "<Separator>",0},
		 { "/File/"N_("Proper_ties"), "<alt>Return",
		   MENU_CALLBACK,  str_properties, "<StockItem>",GTK_STOCK_PROPERTIES},
		 { "/File/sep4",       NULL,
		   NULL,         0,            "<Separator>",0},
		 { "/File/"N_("_Close"),          NULL, 
		   MENU_CALLBACK, str_cancel,   "<StockItem>",  GTK_STOCK_CLOSE},
		};

	if (Owner())
		menubar.Append(sizeof(owned_entries)/sizeof(GtkItemFactoryEntry),
			       owned_entries,
			       *this);
	else
		menubar.Append(sizeof(unowned_entries)/sizeof(GtkItemFactoryEntry),
			       unowned_entries,
			       *this);

	if (! DE_Show_Properties())
	{
		menubar.Remove_Item("/File/Properties"); 
		menubar.Remove_Item("/File/sep3");
	}
}

void Document_Editor_Base::MR_Receive(String message)
{
	if (message == s_open)
	{
		if (modified == 0)
		{
			Load();
		}
		else
		{
			Close_Dialogs(); 
			filename_for= 0;
			file_dialog.Open(File_Dialog::t_open,
					 "asked_filename",
					 *this,
					 Filename());
		}
	}
	else if (message == s_load)
	{
		Load();
	}
	else if (message == s_new)
	{
		if (modified == 0)
			MR_Receive("clear");
		else 
			_GUI_Execute(*this, DE_Mode());
	}
	else if (message == s_asked)
	{
		int choice= ask_dialog.Answer(); 
		Verbose_Int(choice); 
		switch (asked_for_what)
		{
		default:  assert(0);  break; 

		case 0: /* close */

			switch (choice)
			{
			default: assert(0); break;
			
			case GTK_RESPONSE_YES:
				if (Owner() == NULL)
				{
					if (filename.Is())
					{
						if (DO_Save(filename))
						{
							Close(true);
						}
						else
							Save_Error(); 
					}
					else
					{
						asked_save_for= 0; 
						Ask_Save_Name(filename); 
					}
				}
				else
					MR_Receive("ok"); 
				break;

			case GTK_RESPONSE_NO:
				Close(true);
				break;

			case GTK_RESPONSE_CANCEL:
				/* Do nothing */ 
				break;
			}
			break;

		case 1: /* load */
		case 2: /* new */
			switch (choice)
			{
			default:
				assert(0);
				break;

			case GTK_RESPONSE_YES:
				asked_save_for= asked_for_what == 1 ? 2 : 3;
				Ask_Save_Name(filename); 
				break;

			case GTK_RESPONSE_NO:
				if (asked_for_what == 1)
					Ask_Load_Name(); 
				else
					Clear(false);
				break;

			case GTK_RESPONSE_CANCEL:
				/* Do nothing */
				break;
			}
			break;
		}
		ask_dialog.Have_Read(); 
	}
	else if (message == "asked_filename")
	{
		switch (filename_for)
		{
		case 0: /* open */ 
			_GUI_Execute(this,
				     DE_Mode(),
				     file_dialog.Filename()());  
			return;
		case 1: /* load */ 
			Asked_Load_Filename(file_dialog.Filename());
			return;
		case 2: /* save */ 
		{
			String new_filename= file_dialog.Filename();
			assert (new_filename.Is());

			filename= new_filename;
			bool success= Do_Save(); 

			if (Owner() == NULL)
				Filename_Changed(filename);

			if (! success)  return;

			switch (asked_save_for)
			{
			default:assert(0);
				break;

			case 0:	assert (Owner() == NULL);
				Close(true);
				break;

			case 1: /* Do nothing */
				break;

			case 2: Ask_Load_Name(); 
				break;

			case 3: modified= 0;
				DO_Modified_Changed(modified);
				DE_Reset();
				break;

			case 4: /* Do nothing */ 
				break;
			}
			return;
		}
		}
		file_dialog.Have_Read(); 
	}
	else if (message == "save")
	{
		if (Owner())
			Save_Other();
		else
			Save();
	}
	else if (message == "save_as")
		Save_As();
	else 
		Editor_Base::MR_Receive(message);
}

void Document_Editor_Base::Append_Tool_Buttons(Toolbar &toolbar)
{
	if (Owner())  
	{
		ok_button= toolbar.Append(GTK_STOCK_OK, "ok",  _("OK")); 
		apply_button= toolbar.Append(GTK_STOCK_APPLY, "apply",  _("Apply")); 
		toolbar.Append(GTK_STOCK_CANCEL, "cancel",  _("Cancel")); 
		toolbar.Append_Space(); 
		toolbar.Append(GTK_STOCK_CLEAR, "clear", _("Clear"));
	}
	else
	{
		toolbar.Append(GTK_STOCK_NEW, "new",  _(DE_Text_New())); 
		toolbar.Append(GTK_STOCK_OPEN, "open",  _(DE_Text_Open())); 
		toolbar.Append(GTK_STOCK_SAVE, "save", _(DE_Text_Save()));
	}
}

void Document_Editor_Base::Update_Valid(Menubar &menubar)
{
	if (Owner())
	{
		gtk_widget_set_sensitive(ok_button, valid);
		gtk_widget_set_sensitive(apply_button, valid);
		menubar.Set_Sensitive("/File/OK", valid);
		menubar.Set_Sensitive("/File/Apply", valid);
	}
}

void Document_Editor_Base::Save_Error()
{
	String error= Error::Get();
	Close_Dialogs();
	Inform_Error(Format(_("Cannot save \"%s\""),
			    Filename()()),
		     this);
}

void Document_Editor_Base::Ask(int as)
{
	Close_Dialogs();

	String message;
	const char *message_0, 
		*message_2= GTK_STOCK_SAVE,
		*secondary= _("If you don’t save, changes will be discarded.");

	switch (as)
	{
	default:  assert(0);  break;
	case as_close:
		if (Owner())
		{
			message= _("Use the document?");
			message_0= _("Close _without Using");
			message_2= _("_Use");
			secondary= _("If you don’t use the document, changes will be discarded.");
		}
		else
		{
			if (Filename())
				message= Format(_("Save changes to document \"%s\" before closing?"), 
						Filename()());
			else
				message= _("Save the document before closing?");
			message_0= _("Close _without Saving");
		}
		break;
	case as_load:
		if (Owner())  
		{
			message= _("Load file?");
			message_0= _("_Load File");
			message_2= NULL;
			secondary= _("If you load a file, changes will be discarded.");
		}
		else
		{
			if (Filename())
				message= Format(_("Save changes to document \"%s\"?"), 
						Filename()());
			else
				message= _("Save the document?");
			message_0= _("Load File _without Saving");
		}
		break;
	case as_clear:
		if (Owner())
		{
			message= _("Clear the document?");
			message_0= _("_Clear Document");
			message_2= NULL;
			secondary= _("If you clear, changes will be discarded.");
		}
		else
		{
			if (Filename())
				message= Format(_("Save changes to document \"%s\" before clearing?"), 
						Filename()());
			else
				message= _("Save the document before clearing?");
			message_0= _("Clear _without Saving");
		}		
		break;
	}

	ask_dialog.Open(message(),
			s_asked,
			*this,
			message_0,
			message_2,
			secondary);
}

bool Document_Editor_Base::EW_Open()
{
	if (! Window::Open(Wdg_Init_Caption(), DE_Window_Size())) 
		return false;
	if (! box.Open(this))           goto close_window;
	if (! DE_Open(box))goto close_box;  
		
	valid= true; 
	modified= Cleared() ? 0 : 1;
	
	return true; 

 close_box:
	box.Close();
 close_window:
	Window::Close();
	
	return false; 
}

String Document_Editor_Base::Wdg_Init_Caption() const
{
	return EW_Type_Name(); 
}

bool Document_Editor_Base::Wdg_Close_Query()
{
	if (modified >= 2)
	{
		asked_for_what= 0;
		Ask(as_close);
		return false; 
	}
	else
		return true; 
}

GdkPoint Document_Editor_Base::DE_Window_Size() const
{
	return (GdkPoint){dialog_unit*50, dialog_unit*35};   
}

bool Document_Editor_Base::EW_Transient() const
{ 
	return false;  
}

void Document_Editor_Base::EW_Apply()
{
	if (modified <= 1)  return;
	modified= 1; 
	DO_Modified_Changed(modified);
}

bool Document_Editor_Base::EW_Reset()
{
	Clear();
	return true;
}

bool Document_Editor_Base::DE_Set_Args(Open_Info &, bool &)
{
	Verbose_Function(); 
	return true; 
}
