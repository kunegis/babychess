#include "internet_account_editor.hh"

#include "format.hh"

#include <stdio.h>

bool Internet_Account_Editor::OE_Open
(Pointer <Widget> parent,
 const Internet_Account &internet_account)
{
	const char *const texts[4]= 
		{N_("_Host"),
		 N_("_Port"),
		 N_("_User"),
		 N_("Pass_word")};
	if (! table.Open(parent, 4, texts))  return false;
	table.Set(0, internet_account.host);
	table.Set(1, Format("%u", internet_account.port));
	table.Set(2, internet_account.user);
	table.Set(3, internet_account.password);
	gtk_entry_set_visibility(GTK_ENTRY(table.entry(3).Get()), FALSE);
	
	return true; 
}

bool Internet_Account_Editor::OE_Retrieve(Internet_Account &internet_account)
{
	internet_account.host= table[0];
	if (1 != sscanf(table[1](), "%u", &internet_account.port))
		internet_account.port= 5000;
	internet_account.user= table[2];
	internet_account.password= table[3];
	return true;
}
