#include "conf.hh"

#include "read_line.hh"
#include "home.hh"
#include "stream.hh"
#include "settings.hh"
#include "install_dir.hh"
#include "read_line.hh"
#include "gettext.hh"
#include "stream.hh"

#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktable.h>

#define s_edit_changed "edit_changed"

String Conf_Exact_Filename(const char *type)
{
	return String(Home()) + "/"SETTINGS_DIR_NAME"/" + type + "/"CONF_EXACT;
}

String Conf_Exact(const char *type)
{
	String exact_filename= Conf_Exact_Filename(type); 
	Stream stream;
	String ret;
	if (stream.Open(exact_filename(), "r"))  
	{
		Read_Line(stream(), ret); 
		stream.Close(); 
	}
	return ret; 
}

String Conf_Saved_Filename(const char *type)
{
	return String(Settings_Dir())/type+"/"CONF_SAVED;
}

String Conf_Dir::Next()
{
	if (stage == 2)  return "";
	const char *name;
 begin:
	do name= dir.Next();
	while (name && name[0] == '.');
			
	if (name)  
	{
		if (stage == 0)  ++stock_count;
		if (! names.Append(name))
			goto begin;
		Verbose("NEXT *name = %d", *names);
		return name;
	}
	if (stage)  return "";
	stage= 1;
	Object_Reset(Sorted_Dir, &dir, Custom_Dir_Name()(), true);
	if (!dir.Is())  return "";
	goto begin; 
}

void Conf_Dir::Append_Custom(String name)
{
	Verbose("Conf_Dir::Append_Custom(\"%s\")",
		name());
	int size= *names;
	if (! names.Set(size+1))  raise(SIGSEGV);
	names[size]= name; 
	Verbose("\t*names= %d", *names);
}

void Conf_Dir::Remove_Custom(int index)
{
	Verbose("Conf_Dir::Remove_Custom(%d), stock_count = %d",
		index, stock_count);
	assert (index >= stock_count);
	for (int i= index;  i+1 < *names;  ++i)
		names[i]= names[i+1];
	names= -1;
}

Conf_Dir::Conf_Dir(String new_name)
	:  name (new_name)
{ 
	if (! dir.Open(Stock_Dir_Name()(), true))
	{
		stage= 2;
		return;
	}
	stage= 0;
	stock_count= 0;
	Verbose("CONSTR *names = %d", *names);
}

String Conf_Dir::Stock_Dir_Name() 
{
	if (!stock_dir_name)
		stock_dir_name=  Install_Dir() / "share" / name; 
	return stock_dir_name;
}

String Conf_Dir::Custom_Dir_Name() 
{
	if (! custom_dir_name)
		custom_dir_name= String(g_get_home_dir())
			+"/"SETTINGS_DIR_NAME"/"
			+ name;
	return custom_dir_name;
}

String Conf_Dir::Filename(int i)
{
	return  (i < Stock_Count() 
		 ? Stock_Dir_Name() 
		 : Custom_Dir_Name())
		/ 
		Name(i);
}


bool Conf_Menu::Open(Pointer <Widget> parent, 
		     const char *new_type_name, 
		     Saver &new_saver)
{
	/* Widgets */ 
	if (! Table::Open(parent, 2)) return false;
	gtk_table_set_homogeneous(GTK_TABLE(Get()), FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(Get()), dialog_unit/2);
	gtk_table_set_row_spacings(GTK_TABLE(Get()), dialog_unit/2);
	int selection= -1;

	saver= new_saver; 
	initial_exact= Conf_Exact(saver->Get_Type_Name()); 

	Table::fill= true;
	if (! label.Open(this, _("Saved settings")))
		return false;
	Table::space++;

	Table::fill= true; 
	if (! entry.Open(this, Entry::Open_Info
			 ("load",
			  "",
			  false, 0,
			  s_edit_changed))) 
		goto close;
	/* The entry text is set later, when we have determined the
	 * selection.  */
	Table::fill= true;
	if (! info_label.Open(this))  goto close; 
	Table::height= 3+saver->S_Additional_Button_Count();

	if (! list.Open(this, List::Open_Info
			(1, scr_vertical, "list", NULL, true))) 
		goto close;

	type_name= new_type_name; 
	in_list= initial_exact; 

	/* Set list */ 
	{
		const char *type_name= saver->Get_Type_Name();
		conf_dir.reset(new Conf_Dir(type_name));
		String filename;
		int i= 0;
		while (filename= conf_dir->Next())
		{
			if (initial_exact == filename)
				selection= i; 
			list->Set_Row_Count(i+1);
			list->Set(0, i, _(filename()));
			++i;
		}
	}

	if (selection >= 0) list.Set_Selection(selection); 

	/* Set the entry text */
	{
		String text;
		if (selection < conf_dir->Stock_Count() && selection >= 0)
		{
			const char *name= initial_exact();
			const char *t= _(name);
			text= t; 
		}
		else
			text= initial_exact; 
		entry.Set(text); 
	}

	/* Set info text */
	Set_Changed(Get_Changed());

	/* Widgets */ 
	Table::fill= true; 
	if (! save_button.Open(this, "save"  , _("_Save"), true,
	GTK_STOCK_SAVE))  
		goto close; 
	Table::space++;
	Table::fill= true; 
	if (! remove_button.Open(this, "remove", _("_Remove"), true, GTK_STOCK_REMOVE))
		goto close; 
	
	saver->S_Append_Additional_Buttons(*this);

	if (selection < conf_dir->Stock_Count())
	{
		gtk_widget_set_sensitive(remove_button.Get(), false); 
		gtk_widget_set_sensitive(save_button.Get(), false);
	}
	Table::space++;
	if (! blank.Open(this))  return false; 
	
	return true; 

 close:
	Close();
	return false;
}

void Conf_Menu::MR_Receive(String message)
{
	if (message == s_edit_changed)
	{
		Verbose(s_edit_changed);

		String text= entry.Text();
		int found= -1;
		for (int i= 0;  i < list->Row_Count() && found < 0;  ++i)
			if (list->Text(0, i) == text)
				found= i;
		if (found < 0)
		{
			gtk_widget_set_sensitive(remove_button.Get(), false);
			gtk_widget_set_sensitive(save_button.Get(), true);
			Set_Changed(false);
		}
		else 
		{
			list.Set_Selection(found);
			if (found < conf_dir->Stock_Count())
			{
				gtk_widget_set_sensitive(remove_button.Get(), false);
				gtk_widget_set_sensitive(save_button.Get(), false);
			}
			else
			{
				gtk_widget_set_sensitive(remove_button.Get(), true);
				gtk_widget_set_sensitive(save_button.Get(), true);
			}
			Set_Changed(true);
		}
	}
	else if (message == "save")
	{
		String name= entry.Text();
		if (! name.Is())  return;
		if (! Settings_Dir())  return;
		if (! Make_Custom_Dir()) return;
		if (! saver->S_Save(conf_dir->Custom_Dir_Name() / name))  return;
		int size= list->Row_Count();
		Set_Changed(false);
		for (int i= conf_dir->Stock_Count();  i < size;  ++i)
			if (list->Text(0, i) == name)
				return;
		list->Set_Row_Count(size+1);
		list->Set(0, size, name);
		list.Set_Selection(size); 
		gtk_widget_set_sensitive(remove_button.Get(), true);
		conf_dir->Append_Custom(name); 
	}
	else if (message == "list")
	{
		int selection= list->Selection();
		gtk_widget_set_sensitive(remove_button.Get(),
					 selection >= conf_dir->Stock_Count());
		gtk_widget_set_sensitive(save_button.Get(), 
					 selection >= conf_dir->Stock_Count());
		if (selection < 0)  return;
		String name= list->Text(0, selection);
		entry.Set(name); 
		if (! saver->S_Load(conf_dir->Filename(selection)))  
			return;
		in_list= true;
		Set_Changed(false);
	}

	else if (message == "remove")
	{
		int selection= list->Selection();
		Verbose("REMOVE selection = %d", selection);
		if (selection < conf_dir->Stock_Count())  return;
		String name= list->Text(0, selection);
		list->Remove(selection);
		gtk_widget_set_sensitive(remove_button.Get(),
					 list->Selection() >= conf_dir->Stock_Count());
		gtk_widget_set_sensitive(save_button.Get(), 
					 list->Selection() >= conf_dir->Stock_Count());
		String filename= conf_dir->Custom_Dir_Name() / name; 
		unlink(filename()); 
		in_list= false;
		entry.Set("");
		conf_dir->Remove_Custom(selection);
		Set_Changed(false);
	}
	else 
		Table::MR_Receive(message);
}

void Conf_Menu::Wdg_Close()
{
	saver= NULL; 
	Table::Wdg_Close(); 
}

void Conf_Menu::Save()
{
	/* Exact */ 
	if (in_list)
	{
		int selection= list->Selection();
		Save_Exact(conf_dir->Name(selection));
	}
	else
	{
		String name= Conf_Exact_Filename(saver->Get_Type_Name());
		unlink(name()); 
	}

	/* Changed */ 
	if (! Make_Custom_Dir())  return;
	String changed_filename= conf_dir->Custom_Dir_Name()+"/"CONF_CHANGED;
	if (changed)
	{
		Stream s;
		if (! s.Open(changed_filename(), "w")) return;
		s.Close();
	}
	else
	{
		unlink(changed_filename());
	}
}

void Conf_Menu::Cancel()
{
	Save_Exact(initial_exact);
}

bool Conf_Menu::Save_Exact(String exact_name)
{
	Stream file;
	String exact_filename= Conf_Exact_Filename(saver->Get_Type_Name()); 
	if (! file.Open(exact_filename(), "w"))
		return false;

	fprintf(file(),  "%s\n",   exact_name());

	if (! file.Close())  return false;

	return true; 
}

void Conf_Menu::Changed()
{
	Set_Changed(true);
}

bool Conf_Menu::Make_Custom_Dir()
{
	return 0 == access(conf_dir->Custom_Dir_Name()(), F_OK) ||
		0 == mkdir(conf_dir->Custom_Dir_Name()(), 0700);
}

bool Conf_Menu::Get_Changed()
{
	String filename= conf_dir->Custom_Dir_Name()+"/"CONF_CHANGED;
	return 0 == access(filename(), F_OK);
}

void Conf_Menu::Set_Changed(bool new_changed)
{
	assert (Is());
	changed= new_changed;
	info_label.Set(changed ? _("Changed") : "");
}

