#ifndef PREFERENCES_HH
#define PREFERENCES_HH

/* A preference.  Type_Editor is the editor class.  It should implement:  
 *
 *   const char *Default_Conf_Name();
 *         Return the default configuration name or NULL
 */

#include "settings.hh"
#include "owned_editor.hh"
#include "stream.hh"
#include "conf.hh"

#include <sys/stat.h>

template <class Type_Editor>
class Preferences
	:  public Message_Receiver
{
public:
	typedef typename Type_Editor::Edited Type;

	~Preferences()  { }

	void Open(Widget &new_owner,
		  String new_msg_changed)
		{
			owner= new_owner;
			msg_changed= new_msg_changed;
			name= ((Type_Editor*)NULL)->Type_Editor::Get_Type_Name();
			assert (name); 
			String filename= Conf_Saved_Filename(name); 
//			String filename= String(Settings_Dir())/name+"/"CONF_SAVED;
			if (! Read_From_File(filename(), object))
			{
				object.Reset();
				if (Type::Default_Conf_Name())
				{
					String dir= String(Settings_Dir())/name;
					if (!(access(dir(), F_OK) &&
					      0 > mkdir (dir(), 0700)))
					{
						String filename=
							dir+"/"CONF_EXACT;
						Stream file;
						if (file.Open(filename(), "w"))
						{
							String setting= Type::
								Default_Conf_Name();
							fprintf(file(), "%s\n",
								setting());
							file.Close(); 
						}
					}
				}
			}
		}

	void Close()
		{
			editor.Close(); 
			owner= NULL; 
		}

	const Type &Get_Value() const
		{
			return object; 
		}
	const Type &operator () () const
		{
			return Get_Value(); 
		}
	const Type *operator -> () const
		{
			return & Get_Value(); 
		}
	Type &Get_Change_Value() 
		/* The value can be changed, but Changed() must be
		 * called after the change. 
		 */
		{
			return object;
		}

	const Type &Get_Old_Value() const
		/* Only called inside MSG_CHANGED. 
		 */
		{
			return *old_object;
		}
	void Changed()
		/* Must be called after changes were made to the
		 * object using Get_Change_Value().  Doesn't need to
		 * be called if not saved to file. 
		 */
		{ //Settings_Save(object, name);  
			Save();
		}
	void Edit()
		{
			editor.Open("edited", *this, object, owner()); 
		}
	void Caption_Changed()
		{
			if (editor.Is())
				editor.Update_Caption(); 
		}

private:

	Type object;
	std::auto_ptr <Type> old_object;
	Owned_Editor <Type_Editor> editor;
	String msg_changed;
	Pointer <Widget> owner; 
	const char *name;

	void MR_Receive(String message)
		{
			if (message == "edit")
				Edit(); 

			else if (message == "edited")
			{				
				if (old_object.reset(Data_Alloc(Type, object)), 
				    !old_object.get())
					return;
				object= editor.Value();
				owner->MR_Receive(msg_changed);
				old_object.reset();
				editor.Have_Read(); 
				Save();
			}
		}	       
	void Save()
		{
			assert (name);
			String dir= String(Settings_Dir())/name;
			if (access(dir(), F_OK))
				if (0 > mkdir(dir(), 0700))
					return;
			String filename= 
				dir+"/"CONF_SAVED;
			Write_To_File(filename(), object);
		}
};

#endif /* ! PREFERENCES_HH */
