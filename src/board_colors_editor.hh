#ifndef BOARD_COLORS_EDITOR_HH
#define BOARD_COLORS_EDITOR_HH

#include "board_colors.hh"
//#include "board_colors_stream.hh"
#include "check_box.hh"
#include "color_chooser.hh"
#include "gettext.hh"
#include "conf.hh"
#include "label.hh"

class Board_Colors_Editor
	:  public Conf_Editor <Board_Colors>
{
public:
	const char *EW_Type_Name() const  { return _("Board Colors");    }
	const char *Get_Type_Name() const { return "board_theme";       }

private:
	V_Box color_box;
	Title_Label label;
	Color_Chooser color_choosers[c_count];
	Check_Box option_boxes[bt_count];

	bool CE_Open(Pointer <Widget> parent, const Board_Colors &);

	bool OE_Retrieve(Board_Colors &);
	void OE_Update(const Board_Colors &);

	int EW_Use();
};

#endif /* ! BOARD_COLORS_EDITOR_HH */
