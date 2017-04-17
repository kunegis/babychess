#ifndef TIME_WIDGET_HH
#define TIME_WIDGET_HH

#include "labeled_spin_button.hh"

class Time_Widget
	:  public Labeled_Spin_Button
{
public:

	bool Open(Pointer <Widget> parent,
		  int time) /* seconds */
		{
			return Labeled_Spin_Button::Open
				(parent,
				 _("Total _time (min)"),
				 (Spin_Button::Open_Info)
				{NULL, time/60, 1, 1000});
		}

	void Set(int time/* seconds */)
		{
			Labeled_Spin_Button::button.Set(time/60);
		}

	int Time() const
		{
			return Labeled_Spin_Button::button.Value()*60;
		}
};

#endif /* ! TIME_WIDGET_HH */ 
