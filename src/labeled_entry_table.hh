#ifndef LABELED_ENTRY_TABLE_HH
#define LABELED_ENTRY_TABLE_HH

#include "box.hh"
#include "label.hh"
#include "entry.hh"
#include "dynamic.hh"

class Labeled_Entry_Table
	:  public Table
{
public:
	bool Open(Pointer <Widget> parent, 
		  int lines,
		  const char *const *texts,
		  int chars= 10);
	/* TEXTS must not be translated, but should be marked as to be
	 * translated in the source.   */
	void Set(int i, String text)
		{  entries[i]->Set(text);   }
	String operator [] (int i) const
		{  return entries[i]->Text();    }
	const Entry &entry(int i) const 
		{ return entries[i](); }

private:
	Vector <Dynamic <Label> > labels;
	Vector <Dynamic <Entry> > entries;
};

#endif /* ! LABELED_ENTRY_TABLE_HH */ 
