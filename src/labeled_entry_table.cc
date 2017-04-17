#include "labeled_entry_table.hh"

#include "gettext.hh"
#include "format.hh"

#include <gtk/gtklabel.h>
#include <gtk/gtktable.h>

bool Labeled_Entry_Table::Open(Pointer <Widget> parent, 
			       int lines,
			       const char *const *texts,
			       int chars)
{
	if (! Table::Open(parent, 2))  return false;
	gtk_table_set_homogeneous(GTK_TABLE(Get()), FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(Get()), dialog_unit);

	if (! (labels.Set(lines) && entries.Set(lines)))  goto close;
	for (int i= 0;  i < lines;  ++i)
	{
		if (! (labels[i].Alloc() && entries[i].Alloc()))  goto close; 
		if (! labels[i]().Open(this, 
				       Format("%s%s", _(texts[i]), _(":")),
				       0.0, "", true))
			goto close;
		if (! entries[i]().Open(this, Entry::Open_Info("", "")))
			goto close;
		gtk_entry_set_width_chars(GTK_ENTRY(entries[i]->Get()), chars);
		gtk_label_set_mnemonic_widget(GTK_LABEL(labels[i]->Get()), 
					      entries[i]->Get());
	}

	return true;

 close:
	Table::Close();
	return false; 
}

