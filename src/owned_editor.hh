#ifndef OWNED_EDITOR_HH
#define OWNED_EDITOR_HH

#include "dynamic.hh"
#include "widget.hh"

/* TYPE is the editor window type.  TYPE should be derived from
 * Editor_Window <T>.  TYPE should define Get_Value() as a way to
 * access the data in write protected mode.  Type::Edited must be
 * defined as the type that is edited.   
 */

template <class Type_Editor>
class Owned_Editor 
	:  public Dynamic <Type_Editor>
{
public:

	~Owned_Editor() { assert (! Dynamic <Type_Editor> ::Is()); }

	bool Open(String msg,
		  Widget &owner)
		{
			typename Type_Editor::Edited edited;
			edited.Reset();
			return Open(msg, owner, edited, &owner);
		}

	bool Open(String msg,
		  Message_Receiver &owner, 
		  const typename Type_Editor::Edited &edited,
		  Pointer <Widget> parent_window,
		  String caption= "")
		{
			if (Dynamic <Type_Editor> ::Is())
			{
				if ((*this)().Is())
				{
					gtk_window_present(GTK_WINDOW((*this)().Root().Get())); 
					return true;
				}
				else
				{
					Dynamic <Type_Editor> ::Free();
					if (! Dynamic <Type_Editor> ::Alloc())
						return false;
				}
			}
			else
			{
				if (! Dynamic <Type_Editor> ::Alloc())
					return false;
			}
			return (*this)().Open(msg, owner, edited, parent_window, caption);
		}

	const typename Type_Editor::Edited &Value()  
		{ return (*this)().EW_Get_Value(); }

	void Close()
		{
			if (Dynamic <Type_Editor> ::Is())
			{
				if ((*this)().Is())
				{
					bool closed= (*this)().Close_Editor(true);
					assert (closed);
				}
				Dynamic <Type_Editor> ::Free();
			}
		}
	/* Always force */

	bool Have_Read()
		/* Called after the command is received.  Try to close
		 * the window.  Return TRUE if window was closed.
		 * Return FALSE if the window is still open.  
		 */
		{
			if (! Dynamic <Type_Editor> ::Is())
				return true;
			if (! (*this)().Is())
			{
				Dynamic <Type_Editor> ::Free();
				return true; 
			}
			if ((*this)().Close_Editor(false))
			{
				Dynamic <Type_Editor> ::Free(); 
				return true;
			}
			else
			{
				return false; 
			}
		}

	bool Is_Editing() const { return Dynamic <Type_Editor> ::Is() && (*this)().Is();   }
	void Update_Caption()
		{
			if (Dynamic <Type_Editor> ::Is() && (*this)().Is())
			{
				(*this)().Update_Caption(); 
			}
		}
};

#endif /* ! OWNED_EDITOR_HH */
