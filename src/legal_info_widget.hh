#ifndef LEGAL_INFO_WIDGET_HH
#define LEGAL_INFO_WIDGET_HH

#include "box.hh"
#include "generator.hh"
#include "notation.hh"
#include "label.hh"

class Legal_Info_Widget
	:  public V_Box
{
public:

	bool Open(Pointer <Widget> parent);
	void Set(const Generator &,
		 const Notation &);

private:

	Label number_label, castlings_label, hm_label;
};

#endif /* ! LEGAL_INFO_WIDGET_HH */ 
