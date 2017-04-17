#ifndef INTERNET_ACCOUNT_EDITOR_HH
#define INTERNET_ACCOUNT_EDITOR_HH

#include "object_editor.hh"
#include "spin_button.hh"
#include "internet_account.hh"
#include "gettext.hh"
#include "labeled_entry_table.hh"

class Internet_Account_Editor
	:   public Object_Editor <Internet_Account> 
{
public:
	const char *EW_Type_Name() const { return _("Internet Chess Account");  }
	const char *Get_Type_Name() const { return "internet_account";          }

private:
	Labeled_Entry_Table table;
	
	bool OE_Open(Pointer <Widget> parent, const Internet_Account &);
	bool OE_Hide_Apply() const { return true; }
	const char *OE_OK_Text() const {return _("_Login"); }
	

	int EW_Use()
		{  return use_ok;  }
	
	bool EW_Change_Value(const Internet_Account &) 
		{
			assert(0);
			return false;
		}

	bool OE_Retrieve(Internet_Account &);
};

#endif /* ! INTERNET_ACCOUNT_EDITOR_HH */
