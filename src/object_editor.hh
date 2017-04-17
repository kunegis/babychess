#ifndef OBJECT_EDITOR_HH
#define OBJECT_EDITOR_HH

/* A window that edits an object.  The window is always owned by
 * another window.   Objects cannot be saved to file by the user,
 * e.g. Notation_Settings.  */

#include "editor.hh"
#include "bevel.hh"
#include "text_button.hh"

#include <gtk/gtkdialog.h>

class Dialog
	:  virtual public Widget
{
public:
	bool Dialog_Open(bool hide_apply,
			 const char *ok_text= NULL);

private:
	int hide_apply; 
	GtkWidget *box;

	void Wdg_Append_Child(Widget &widget);

	static void Signal_Response(GtkDialog *,
				    gint,
				    gpointer);
};

template <class Type>
class Object_Editor
	:  public Editor <Type> ,
	   public Dialog
{
public:
	virtual bool OE_Retrieve(Type &object)= 0; 
	/* Take the current settings and put them in OBJECT.  */
	virtual void OE_Update(const Type &) {}

	const Type &EW_Get_Value() 
		{
			if (Is())   OE_Retrieve(object);
			return object;
		}

private:
	Type object; 

	virtual bool OE_Get_Apply() const { return false;  }
	virtual bool OE_Open(Pointer <Widget> parent, const Type &)= 0; 
	virtual void OE_OK() {}
	virtual bool OE_Hide_Apply() const { return false; }
	virtual const char *OE_OK_Text() const
		{ return NULL; }

	bool EW_Open()
		{
			if (! Dialog::Dialog_Open(OE_Hide_Apply(),
						  OE_OK_Text()))     
				return false; 
			if (! OE_Open(this, object))    return false; 
			return true; 
		}
	void EW_Set_Value(Pointer <const Type> value)
		{
			if (value.Is()) object= value();
			else            object.Reset();
		}
	void EW_OK()
		{
			OE_Retrieve(object); 
			OE_OK();
		}

	GdkPoint DE_Window_Size() const { return (GdkPoint){-1, -1}; }
	void Wdg_Close()
		{
			OE_Retrieve(object);
			Editor <Type> ::Wdg_Close(); 
		}
}; 

#endif /* ! OBJECT_EDITOR_HH */ 
