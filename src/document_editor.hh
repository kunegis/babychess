#ifndef DOCUMENT_EDITOR_HH
#define DOCUMENT_EDITOR_HH

/* Implementation of the document logic:  current filename, whether is was
 * modified, query the filename, query whether to close the document...
 */

#include "file_dialog.hh"
#include "stream_file.hh"
#include "ask_dialog.hh"
#include "process_open_info.hh"
#include "toolbar.hh"
#include "window.hh"
#include "editor.hh"
#include "box.hh"
#include "menu.hh"

#if 0 /* moved to stream_file */
/* This functions is implemented with "Type *" as the second argument
 * in some modules that use this module.  */
Inline bool Check_Filename(String, void *)
{
	return true;
}
#endif /* 0 */

class Document_Editor_Base
	:  public Window,
	   virtual public Editor_Base
{
public:
	/* Return value of DO_Load() */
	enum Load_Result
	{
		lr_error= 0, 
		lr_success= 1,
		lr_wrong_type= 2,
		/* File was loaded, but is of wrong type, thus is
		 * cannot be overwritten. 
		 */
	};

	enum
	{
		/* Ask for what */
		as_close= 0,
		as_load,
		as_clear,
	};
	
	void Open(String filename);

	void Load();
	bool Save();
	/* Result:  Whether saving was done.  */
	void Save_As();
	void Save_Other();
	bool Asked_Load_Filename(String filename);
	void Asked(int choice);
	/* Return a constant from CHOICE:  
	 * 0: no,
	 * 1: cancel
	 * 2: save/yes
	 */
	void Asked_Overwrite(int choice); 
	bool Load_File(String filename, bool anonymous= false);
	/* Load a specific file.  Return FALSE on error. 
	 */
	void Modify();
//	void Unmodify();
//	bool Close_Query();
	void Clear(bool ask= true);

	bool Has_Filename() const
		{ return filename.Is();  }
	String Filename() const
		{ return filename;  }
	int Modified() const
		{ return modified;  }

	typedef Process_Open_Info Open_Info; 

	void Filename_Changed(String filename);
	void Save_Error();
//	void DO_Close();

	void MR_Receive(String);

	void Append_Menu(Menubar &menubar);
	void Append_Tool_Buttons(Toolbar &);
	void Set_Valid(bool new_valid, Menubar &menubar)
		{
			valid= new_valid;
			Update_Valid(menubar);
		}

protected:

	virtual bool DE_Set_Args(Open_Info &open_info,
				 bool &anonymous);
		/* Called before opening to set args. OPEN_INFO contains the
		 * program name, and all options that were parsed should be
		 * removed.  Return FALSE if the object should not be opened
		 * (wrong options etc...)  The object also has to remember
		 * somehow that options were passed.  (e.g. EW_Set_Value()
		 * should be ignored later on if the options specified an
		 * initial value (e.g. a position in FEN).  Set ANONYMOUS to
		 * true when encountering the -a option. 
		 */

	void Close_Dialogs();

	Menubar menubar; 
	File_Dialog file_dialog;
	int filename_for;
	/* 0: open
	 * 1: load
	 * 2: save
	 */
	Ask_Dialog ask_dialog; 
	V_Box box; 

private:

	GtkWidget *ok_button, *apply_button; 
	bool valid; 

	virtual const char *DE_Mode() const= 0;
	/* Return the mode passed to the BabyChess executable to edit programs
	 * of this type.  
	 */

	/* The next three return etxt for toolbar tooltips (in sentence
	 * capitalization) */ 
	virtual const char *DE_Text_New() const= 0;
	/* return "New OBJECT" */ 
	virtual const char *DE_Text_Open() const= 0;
	/* return "Open an OBJECT" */ 
	virtual const char *DE_Text_Save() const= 0;
	/* return "Save this OBJECT" */ 

	virtual bool DE_Open(Pointer <Box> parent)= 0;
	virtual GdkPoint DE_Window_Size() const;
	virtual void DE_Reset()= 0;
	/* Clear object when window is open. */
	virtual bool DE_Show_Properties() const
		{  return false; }
	/* If TRUE is returned, a "Properties" item is showed in the
	 * "File" menu which generates the "properties" message. */
	virtual bool DE_Show_Export() const
		{  return false; }

	String Wdg_Init_Caption() const;

	void Ask_Load_Name();
	void Ask_Save_Name(String initial_name);
	void Ask(int);

	bool EW_Open();
	bool EW_Transient() const;
	void EW_Apply();
		/* When the value is applied.   */
	bool EW_Reset();

	bool Wdg_Close_Query();

	void Update_Valid(Menubar &);
	
	String filename;
	/* If empty, no filename specified.
	 */
	int modified;
	/* Whether the document was modified. 
	 * 0: not modified
	 * 1: not modified, but not in reset state
	 * 2: modified
	 */

	int asked_for_what;
	/* What was the reason for asking.
	 *    0:   closing 
	 *    1:   loading another filename 
	 *    2:   new
	 */
	int asked_save_for;
	/* What was the reason for asking the save filename.
	 *    0:  Save before closing.
	 *    1:  Save normally. 
	 *    2:  Save before loading. 
	 *    3:  Save before new.  (is this even used)
	 *    4:  Save before saving as other. 
	 */

	virtual Load_Result DO_Load(String filename)= 0;
	virtual bool DO_Save(String filename)= 0;
	virtual void DO_Modified_Changed(int /* new_modified */ ) {}
	/* Called when the flag MODIFIED changes.  Can be used
	 * e.g. to display "modified" in the statusbar of a document
	 * window. 
	 */

	bool Do_Save();
	/* Save to file. */
};

template <class Type>
class Document_Editor
	:  public Document_Editor_Base,
	   public Editor <Type> 
{
public:

	bool Open(Open_Info args)
		/* Open with no owner */
		{
			Verbose_Function();
			Verbose_Int(args.argc);

			bool anonymous= false;

			/* Each editor may have it's own special arguments */ 
			if (! DE_Set_Args(args, anonymous))     return false; 

			Verbose_Int(args.argc);

			/* Strip the program name from ARGS */
			--args.argc;
			++args.argv; 

			/* Without arguments, open a new document */ 
			if (args.argc == 0)          
			{
				return Editor <Type> ::Open("");
			}

			/* Else assume a filename */ 
			if (! Load_File(args.argv[0], anonymous))
				return false;
			
			if (! Editor <Type> ::Open
			    (-1, NULL, this->EW_Get_Value(), NULL, "", true))
				return false;

			if (! anonymous)
				Filename_Changed(Filename()); 
			else 
				Modify();

			return true; 
		}

	bool Open(String msg_edited,
		  Message_Receiver &owner, 
		  Pointer <const Type> initial_value= NULL,
		  Pointer <Widget> parent_window= NULL,
		  String caption= "")
		/* with owner */
		{
			return Editor <Type> ::Open
				(msg_edited, 
				 owner,
				 initial_value,
				 parent_window,
				 caption); 
		}

	void Wdg_Close()
		{
			Close_Dialogs();
			Editor <Type> ::Wdg_Close();
		}

private:
	bool DO_Save(String filename)
		{
			return Write_To_File(filename(),
					     this->EW_Get_Value()); 
		}

	Load_Result DO_Load(String filename)
		{
			Type object;
			Load_Result result;

			/* Check whether filename matches the usual filename for
			 * that data type.  */
			bool cf= Check_Filename(filename, (Type *)NULL);
			Verbose_Int(cf);
			if (cf)
			{
				if (! Read_From_File(filename(), object))
					result= lr_error;
				else
					result= lr_success;
			}

			/* Try to load the file as something else */
			else if (EW_Try_Load(filename, object))
				result= lr_wrong_type;

			/* Try loading anyway */
			else
			{
				if (! Read_From_File(filename(), object))
					result= lr_error;
				else
					result= lr_success;
			}

			if (result != lr_error)
			{
				if (Widget::Is())
				{
					if (! EW_Change_Value(object))
					{
						result= lr_error;
					}
				}
				else
				{
					EW_Set_Value(object); 
				}
			}

			return result;
		}
};

#endif /* ! DOCUMENT_EDITOR_HH */
