#include "editor.hh"

#include <errno.h>
#include <gtk/gtkwindow.h>

void Editor_Base::Update_Caption()
{
	String caption= Get_Full_Caption();
	gtk_window_set_title(GTK_WINDOW(Root().Get()),
			     caption()); 
	EW_Caption_Changed();
}

String Editor_Base::Get_Full_Caption() const
{
	assert (caption.Is()); 
	String result= caption;
	if (parent_window.Is() && ! EW_Transient())
	{
		result += " – "; /* UTF-8 ndash */ 
		result += gtk_window_get_title(GTK_WINDOW(parent_window->Root().Get()));
	}
	return result; 
}

void Editor_Base::Set_Caption(String new_caption)
{
	caption= new_caption;
	Update_Caption();
}


const char *Editor_Base::EW_Type_Name() const
{
	return "Object";
}

void Editor_Base::EW_Caption_Changed()
{
}

void Editor_Base::MR_Receive(String message)
{
	if (message == "cancel")
	{
		Close();
	}
	else if (message == "clear")
		EW_Reset(); 
	else if (message == "ok")
	{
		switch (EW_Use())
		{
		case use_ok:
			must_close= true;
			if (owner.Is())
			{
				EW_OK();
				owner().MR_Send(msg_edited); 
				if (parent_window.Is())
					gtk_window_present
						(GTK_WINDOW(parent_window->Root().Get()));
			}

			/* fall through */
		case use_no:
			EW_Saved_Value_Free();
			cancel= false;
			Close();
			break;

		case use_retry:
			break;
		}
	}
	else if (message == "apply")
	{
		switch (EW_Use())
		{
		case use_ok:
			must_close= false;
			was_applied= true;
			EW_OK(); 
			owner().MR_Send(msg_edited);
			EW_Apply(); 
			break;

		default:
			break;
		}
	}

	else 
		Widget::MR_Receive(message);
}

int Editor_Base::EW_Use() 
{
	return use_ok; 
}

void Editor_Base::EW_Apply()
{
}

bool Editor_Base::EW_Reset()
{
	errno= ENOSYS;
	return false;
}

bool Editor_Base::Do_Open(String new_msg_edited,
				 Pointer <Message_Receiver> new_owner,
				 Pointer <Widget> new_parent_window,
				 String new_caption)
{
	Verbose_Function();

	msg_edited= new_msg_edited;

	owner= new_owner;
	parent_window= new_parent_window;
	/* PARENT_WINDOW must be set before calling SET_CAPTION
	 * because SET_CAPTION accesses PARENT_WINDOW. 
	 */

	if (! EW_Open())
	{
		owner= NULL;
		parent_window= NULL;
		return false; 
	}


	Set_Caption(new_caption.Is() ? new_caption : EW_Type_Name()); 

	if (parent_window.Is() && EW_Transient())
		gtk_window_set_transient_for(GTK_WINDOW(Root().Get()),
					     GTK_WINDOW(parent_window->Root().Get()));

	cancel= true;
	
	gtk_widget_show(Root().Get());

	return true;
}
