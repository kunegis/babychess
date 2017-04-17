#ifndef POSITION_EDITOR_HH
#define POSITION_EDITOR_HH

#include "position_stream.hh"
#include "document_editor.hh"
#include "position.hh"
#include "radio_group.hh"
#include "toolbar.hh"
#include "statusbar.hh"
#include "text_button.hh"
#include "board.hh"
#include "entry.hh"
#include "legal_move_list.hh"
#include "notation_editor.hh"
#include "preferences.hh"
#include "legal_info_widget.hh"
#include "option_menu.hh"
#include "labeled_spin_button.hh"
#include "piece_radio_button.hh"
#include "notation_settings_editor.hh"
#include "material_widget.hh"

class Position_Editor
	:  public Document_Editor <Position> 
{
public:
	const Position &EW_Get_Value();

	static bool next_turned;
	/* Can be set before opening a new Position_Editor.
	 * Determines whether the board in the new editor will be
	 * turned.   */

private:
	Position position;
	/* Not in legal mode
	 * MOVED[king squares] is always 0.
	 * MOVED[rook squares] may be 0 even there is no rook on the
	 * rook square.  In this case, the castling will be marked as
	 * available when a rook is added. 
	 * MATERIAL is always correct. 
	 */

	bool legal;
	/* Whether the position is legal.  Set by Update_Legality().  
	 */

	int8_t ep_index[8];
	/* Set by Update_EP(), read by MR_Receive("ep").
	 * Index is of items in EP_CHOOSER.  Value is file of move. 
	 */

	Preferences <Notation_Editor> notation_preferences;
	Preferences <Notation_Settings_Editor> notation_settings_preferences; 

	Toolbar toolbar;
	H_Box middle_box;
	V_Box side_box;
	Framed_Radio_Group color_chooser;
	H_Box type_boxes[3];
	Piece_Radio_Button type_buttons[12];
	H_Box last_move_box;
	Label last_move_label; 
	Option_Menu last_move_menu; 
	Title_Label castling_label, type_label;
	Table castling_box;
	Check_Box castling_check_boxes[4];
	Labeled_Spin_Button previous_moves;
	Legal_Info_Widget legal_info;
	Legal_Move_List legal_move_list;
	H_Box hm_box;
	Label hm_text; 
	Spin_Button hm_edit;
	H_Box fen_box;
	Label fen_text;
	Entry fen_entry;
	Text_Button fen_button;
	Board board;
	Statusbar statusbar;
	Material_Widget material_widget;

	bool DE_Open(Pointer <Box>);

	const char *EW_Type_Name() const 
		{ return _("Chess Position");   }

	int EW_Use();
	void EW_Set_Value(Pointer <const Position>);
	bool EW_Change_Value(const Position &);
	void DE_Reset();
	const char *DE_Mode() const
		{  return "--edit-position";  }
	virtual const char *DE_Text_New() const {return N_("New position");}
	virtual const char *DE_Text_Open() const{return N_("Open a position");}
	virtual const char *DE_Text_Save() const{return N_("Save this position");}

	bool EW_Try_Load(String, Pointer <Position>);
	void EW_Caption_Changed();

	void MR_Receive(String);

	void DO_Modified_Changed(int);

	void Update_Board();
	/* When POSITION has changed.  Update the board and
	 * COLOR_CHOOSER.  Calls all other Update...() functions. 
	 */

	void Update_Legality();
	/* Set LEGAL and update status line.
	 */

	void Update_EP();
	/* Update ep chooser.  To be called after Update_Legality()
	 * because is reads LEGAL. 
	 */
	void Update_FEN();
	/* Update FEN edit text.  Must be called after Update_EP(). 
	 */
	void Update_Castlings();
	/* Update castling check boxes. 
	 */
	void Update_PM();
	void Update_Legal();
	/* Update legal move list if visible.  "legal" must be set. 
	 */
	void Insert_New_Piece(int square);
	void Wdg_Close();
	void Retrieve_PM();
	void Remove_Piece(int square);
	void Normalize_Castlings();
	void Update_HM();
	void Update_Material(); 

	static int Castling_Moved(int index)
		{
			return index / 2 * 3 + 2 - index % 2 * 2;
		}
};

#endif /* ! POSITION_EDITOR_HH */
