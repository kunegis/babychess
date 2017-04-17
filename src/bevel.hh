#ifndef BEVEL_HH
#define BEVEL_HH

#include "widget.hh"

class Bevel
	:  public Widget
{
public:
	
	bool Open(Pointer <Widget> parent,
		  bool direction_h= true);
};

#endif /* ! BEVEL_HH */
