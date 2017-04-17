#include "legal_info_widget.hh"

#include "format.hh"
#include "gettext.hh"

bool Legal_Info_Widget::Open(Pointer <Widget> parent)
{
	if (! V_Box::Open(parent))     return false;
	if (! number_label.Open(this, "", 0.0, "weight = \"bold\""))    
		return false;
	if (! castlings_label.Open(this)) return false;
	if (! hm_label.Open(this))        return false;
	return true;
}

void Legal_Info_Widget::Set(const Generator &generator,
			    const Notation &notation)
{
	/* Number of legal moves */ 
	number_label.Set(Format(ngettext("%d legal move", "%d legal moves",
					 *generator),
				*generator));

	/* Castlings */ 
	String castlings;
	for (int i= 0;  i < 4;  ++i)
	{
		bool possible= ! 
			(generator.Get_Position().Moved(1 + i / 2 * 3) ||
			 generator.Get_Position().Moved(2 + i / 2 * 3 - (i % 2 * 2)));
		if (possible)
		{
			if (castlings.Is())
				castlings += ", ";
			castlings += notation.texts[i_color + i / 2];
			castlings += notation.texts[i_P + 5 - i % 2]; 
		}
	}
	if (castlings.Is())
	{
		castlings_label.Set(Format("%s %s",
					   _("Castlings:"),
					   castlings()));
	}
	else
		castlings_label.Set(_("No castlings"));

	/* Halfmove clock */ 
	String hm= Format("%s  %d",
			  _("Draw clock:"),
			  generator.Get_Position().Halfmove_Clock() / 2);
	if (generator.Get_Position().Halfmove_Clock() >= 100)
	{
		hm += " (";
		hm += _("draw");
		hm += ")"; 
	}

	hm_label.Set(hm);
}
