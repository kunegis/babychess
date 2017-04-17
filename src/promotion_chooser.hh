#ifndef PROMOTION_CHOOSER_HH
#define PROMOTION_CHOOSER_HH

#include "piece_radio_button.hh"
#include "box.hh"
#include "label.hh"
#include "position.hh"

class Promotion_Chooser
	:  public H_Box
{
public:
	bool Open(Pointer <Widget> parent);
	int Selection();
	/* Read the selection and reset the chooser to the queen */ 
	void Update(const Position &);

private:
	Piece_Radio_Button buttons[4];
};

#endif /* ! PROMOTION_CHOOSER_HH */ 
