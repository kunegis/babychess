#ifndef EDITOR_HH
#define EDITOR_HH

/* A window that edits an object.  Editor is a base class of: 
 *
 * - Object_Editor:  Objects that can be edited but are not treated as a
 *                   document, e.g. Board_Colors. 
 *
 * - Document_Editor:  Objects that are written to files, e.g. Position.
 */

#include "widget.hh"

#include <errno.h>
#include <memory>

class Editor_Base
	:  virtual public Widget
{
public:

	enum {use_ok,     /* OK                                  */
	      use_no,     /* Finish editing, don't save changes  */
	      use_retry}; /* Continue editing                    */

	String caption; 
	Pointer <Message_Receiver> owner;
	bool must_close;
	/* Set before MSG_EDITED is sent to owner.  TRUE when window
	 * must be closed after the owner read the value. 
	 */
	String msg_edited;
	/* Sent to owner after editing.  */
	Pointer <Widget> parent_window; 
	/* Widget that is activated when editing is finished.  */
	bool was_applied; 

	void Update_Caption();
	String Get_Full_Caption() const;
	void Set_Caption(String new_caption);
	Pointer <Message_Receiver> Owner() const
		{ return owner; }

	virtual const char *EW_Type_Name() const;
	/* The full type name, all words capitalized.
	 * (e.g. "Chess Game") */

	virtual void EW_Caption_Changed();
	virtual int EW_Use();
	/* Called before the value is used.  Return one of the use_*
	 * constants.  
	 */
	virtual void EW_Apply();
	/* When the value is applied. */
	virtual bool EW_Reset();
	/* Make new when window is open. */
	virtual bool EW_Transient() const { return true;  }
	/* Return whether the editor should be transient to it's parent. */  
	virtual bool EW_Open()= 0;
	/* Open the editor widget.  Set_Value() has already been called at
	 * this moment.  
	 */
	

	void MR_Receive(String);

protected:
	bool cancel;
	bool cleared;

	bool Do_Open(String new_msg_edited,
		     Pointer <Message_Receiver> new_owner,
		     Pointer <Widget> new_parent_window,
		     String new_caption);
	/* The value has already been set. 
	 */
	bool Cleared() const 
		{return cleared; }
	
private:

	virtual void EW_Saved_Value_Free()= 0;
	/* Free SAVED_VALUE.  Only implemented in Editor <> .
	 */
	virtual void EW_OK() {}
	/* Called before closing when the value is used. 
	 */
	virtual void EW_Cancel() {}
	/* When cancelling, before closing the window. 
	 */
};

template <class Type>
class Editor
	:  virtual public Editor_Base
{
public:

	typedef Type Edited;

	virtual const Type &EW_Get_Value() = 0;
	/* May be called after widget was closed.  If that is the
	 * case, EW_OK() is called before closing. 
	 */

	void Wdg_Close()
		{
			Widget::Wdg_Close();
			if (saved_value.get())
			{
				assert (owner.Is()); 
				if (was_applied)
				{
					EW_Set_Value(*saved_value);
					owner().MR_Send(msg_edited); 
				}
				saved_value.reset(); 
			}
		}

	bool Open(String new_msg_edited,
		  Pointer <Message_Receiver> new_owner= NULL, 
		  Pointer <const Type> initial_value= NULL,
		  Pointer <Widget> new_parent_window= NULL,
		  String new_caption= "",
		  bool ignore_value= false) 
		/* If OWNER is not NULL, MSG_EDITED is sent to OWNER when
		 * editing is finished.  If IGNORE_VALUE is TRUE,
		 * Editor_Set_Value() must be called before Open(). 
		 */
	{
		assert ((! new_caption.Is()) || new_owner.Is()); 

		if (! ignore_value)
		{
			if (initial_value.Is())
			{
				saved_value.reset(new Type(initial_value()));
				was_applied= false;
			}
			EW_Set_Value(initial_value);
		}
		cleared= ignore_value || !initial_value; 
		
		bool ret= Do_Open(new_msg_edited, new_owner, new_parent_window, new_caption);
		return ret;
	}

	bool Close_Editor(bool force)
		/* Return whether was closed. */
		{
			if (must_close || force)
			{
				Widget::Close(true);
				/* force closing */

				owner= NULL; 
				parent_window= NULL; 
				
				return true;
			}
			else
				return false;
		}

protected:


	virtual void EW_Set_Value(Pointer <const Type> value)= 0;
	/* Initialize editor with VALUE.  The window is not open at
	 * that moment.  If VALUE is NULL, set to default value.
	 * Must be called before Open() when IGNORE_VALUE is set in
	 * Open().  Is also called after the window was closed when
	 * cancelling. 
	 */

	virtual bool EW_Change_Value(const Type &) 
		/* Change value when editor is open.  Return TRUE on success.
		 * Not implemented by default.  
		 */
		{
			errno= ENOSYS;
			return false; 
		}

	virtual bool EW_Try_Load(String /* filename */ ,
				 Pointer <Type> /* object */ ) 
		/* Try to load FILENAME, which is not of type Type.  Return
		 * TRUE on success.  Fill OBJECT with content of
		 * FILENAME.   */
		{ 
			return false;  
		}

private:
    
	std::auto_ptr <Type> saved_value; 

	void EW_Saved_Value_Free()
		{
			saved_value.reset(); 
		}
};

#endif /* ! EDITOR_HH */
