#ifndef CONF_HH
#define CONF_HH

/* Configurations--List of saved settings. 
 */

/* Each is in a directory in ~/.babychess, for instance
 * ~/.babychess/notation.  Files there are one custom saved settings.
 * Files with dots are special:
 *   .saved:   Always the current settings
 *   .exact:   Contains the name of the setting. 
 *   .changed: If it exists, the settings was changed. 
 */
/* Managed settings that can be saved.  Stock settings are found in 
 * "share/MODULE/", where MODULE is e.g. "notation".  Settings saved
 * by the user are saved in "~/.babychess/MODULE.saved/". 
 */


#include "list.hh"
#include "entry.hh"
#include "text_button.hh"
#include "box.hh"
#include "label.hh"
#include "sorted_dir.hh"
#include "install_dir.hh"
#include "filename.hh"
#include "settings.hh"
#include "vector.hh"
#include "object_editor.hh"
#include "stream_file.hh"
#include "box.hh"


#include <memory>

#define CONF_SAVED   ".saved" 
#define CONF_EXACT   ".exact" 
#define CONF_CHANGED ".changed" 
 
String Conf_Exact(const char *type);
String Conf_Saved_Filename(const char *type);

class Conf_Dir
/* Reads the names of all configurations.  There are two types of
 * configurations:  the stock configurations (saved in PREFIX/share/babychess),
 * and the custom configurations (saved in ~/.babychess). 
 */
{
public: 
	Conf_Dir(String new_name);
	int Stock_Count() const 
		{return stock_count;}
	String Name(int i) const
		{ return names[i];  }
	String Stock_Dir_Name();
	String Custom_Dir_Name();
	String Next();
	void Append_Custom(String name);
	void Remove_Custom(int); 
	String Filename(int);

private:
	Sorted_Dir dir;
	int stage;
	/* 0:  stock
	 * 1:  custom
	 */
	int stock_count;
	String name; 
	String stock_dir_name, custom_dir_name; 
	Vector <String> names;
	/* English names  */ 
};




class Conf_Menu
	:  public Table
{
public:

	class Saver
		:  virtual public Pointed
	{
	public:
		virtual bool S_Save(String) { return false; }
		virtual bool S_Load(String) { return false; }
		virtual const char *Get_Type_Name() const { return NULL; }
		virtual int S_Additional_Button_Count() const 
			{ return 0; }
		virtual void S_Append_Additional_Buttons(Table & /* parent */ )
			{ }
		/* Must increment SPACE and set FILL to TRUE before opening each
		 * button. 
		 */
	};

	bool Open(Pointer <Widget> parent, 
		  const char *type_name, 
		  Saver &saver);
	void Save();
	/* Must be called if the value is used.  */
	void Cancel();
	/* Restore the initial valeu of EXACT_NAME. 
	 */
	void Changed();
	/* To be called when settings are changed in the window.  */ 

private:
	List list;
	Title_Label label;
	Entry entry;
	Label info_label;
	Text_Button save_button, remove_button; 
	Label blank;

	const char *type_name;
	Pointer <Saver> saver; 
	bool in_list;
	String initial_exact;
	std::auto_ptr <Conf_Dir> conf_dir; 
	bool changed;

	void MR_Receive(String);
	void Wdg_Close();

	bool Save_Exact(String exact_name);
	bool Make_Custom_Dir();
	bool Get_Changed();
	void Set_Changed(bool);
};

template <class Type>
class Conf_Editor 
	:  public Object_Editor <Type> ,
	   public Conf_Menu::Saver 
{
private:
	H_Box main_box;
	Conf_Menu conf_menu; 

	virtual bool CE_Open(Pointer <Widget> parent,
			     const Type &)= 0;
	/* PARENT is an H_Box.  */

	bool OE_Open(Pointer <Widget> parent, 
		     const Type &object)
		{
			if (! main_box.Open(parent, Widget::dialog_unit*3/2))  
				return false;
			if (! CE_Open(main_box, object))
				return false;
			if (! conf_menu.Open(main_box, "Board_Colors", *this))  
				return false; 
			return true;
		}
	void OE_OK()
		{
			conf_menu.Save();
		}

	bool S_Save(String filename)
		{
			Type object;
			OE_Retrieve(object); 
			return Write_To_File(filename(), object); 
		}

	bool S_Load(String filename)
		{
			Type object; 
			if (! Read_From_File(filename(), object))
				return false;
	
			OE_Update(object); 
			return true;
		}

	void EW_Cancel() 
		{
			conf_menu.Cancel();
		}
	void MR_Receive(String message)
		{
			if (message == "changed")
			{
				conf_menu.Changed();
			}
			else
				Object_Editor <Type> ::MR_Receive(message);
		}
};

#endif /* ! CONF_HH */ 
