#ifndef SCROLLED_HH
#define SCROLLED_HH

//#include "scroll_widget.hh"
#include "containing.hh"

enum Scroll_Direction {scr_horizontal= (1 << 0),
		       scr_vertical  = (1 << 1),
		       scr_both= scr_horizontal | scr_vertical};

class Scroll_Widget
	:  public Widget 
{
public:
	class Open_Info
	{
	public: 
		Scroll_Direction scroll_direction;
		Open_Info(Scroll_Direction new_scroll_direction= scr_both)
			:  scroll_direction(new_scroll_direction)
			{
			}
	};

	bool Open(Pointer <Widget> parent,
		  Open_Info oi= Open_Info());
};

template <class Type> 
class Scrolled
	:  public  Containing <Scroll_Widget, Type>
{
};

#endif /* ! SCROLLED_HH */
