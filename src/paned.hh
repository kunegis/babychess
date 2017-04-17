#ifndef PANED_HH
#define PANED_HH

#include "widget.hh"

class Paned
	:  public Widget
{
public:    void Wdg_Append_Child(Widget &widget/*, bool*/);
protected: int next;
};

class H_Paned
	:  public Paned
{
public:
	bool Open(Pointer <Widget> parent);
};

class V_Paned
	:  public Paned
{
public:
	bool Open(Pointer <Widget> parent);
};

#endif /* ! PANED_HH */ 
