#ifndef LABELED_SPIN_BUTTON_HH
#define LABELED_SPIN_BUTTON_HH

#include "box.hh"
#include "spin_button.hh"
#include "label.hh"

class Labeled_Spin_Button
	:  public H_Box
{
public:
	Spin_Button button;

	bool Open(Pointer <Widget> parent,
		  const char *label_text,
		  Spin_Button::Open_Info);

private:
	Label label;
};


#endif /* ! LABELED_SPIN_BUTTON_HH */
