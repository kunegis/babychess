#ifndef WINDOW_HH
#define WINDOW_HH

#include "widget.hh"

class Window
	:  virtual public Widget
{
public:
	bool Open(String caption, GdkPoint size); 
};

#endif /* ! WINDOW_HH */
