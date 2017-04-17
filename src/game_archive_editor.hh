#ifndef GAME_ARCHIVE_EDITOR_HH
#define GAME_ARCHIVE_EDITOR_HH

//#include "game_archive.hh"
#include "game_archive_stream.hh"
#include "document_editor.hh"
#include "statusbar.hh"
#include "game_editor.hh"
#include "toolbar.hh"
#include "gettext.hh"

class Game_Archive_Editor
	:  public Document_Editor <Game_Archive> 
{
private:
	class Edited_Game
	{
	public:

		int game_index;
		Owned_Editor <Game_Editor> game_editor;
	};

	Toolbar toolbar; 
	GtkWidget *add_button, *remove_button, *edit_button;
	List game_list;
	Statusbar statusbar;

	Game_Archive game_archive;
	Vector <Edited_Game> edited_games;
	File_Dialog file_dialog;
	Vector <int> sort_order[7], sort_iorder[7];
	int sort_index;
	/* -1 when not sorted, or the index (0..6) of standard PGN
	 * order. */

	static Game_Archive_Editor *editor;

	bool DE_Open(Pointer <Box>);

	const char *EW_Type_Name() const 
		{  return _("Chess Game Archive");  }

	void EW_Set_Value(Pointer <const Game_Archive>);
	const Game_Archive &EW_Get_Value();
	bool EW_Change_Value(const Game_Archive &);
	void EW_Caption_Changed();

	void DE_Reset();
	const char *DE_Mode() const
		{ return "--edit-game-archive";   }
	const char *DE_Text_New() const {return N_("New chess game archive");}
	const char *DE_Text_Open() const{return N_("Open a chess game archive");}
	const char *DE_Text_Save() const{return N_("Save the chess game archive");}

	void MR_Receive(String);

	void Wdg_Close();

	void DO_Modified_Changed(int);

	void Update_List(int from= 0,
			 int new_sort_index= -1);
	/* Sort by column SORT_INDEX */
	void Update_Count();
	void Update_Full(bool full);
	void Close_Editors();
	void Edit(int game_index= -1,
		  bool set_selection= true);
	/* Edit game INDEX.  If -1, use selection of list box.   */
	void Update_Game_Info(int list_index,
			      int game_index);
	void Update_Edited_Game_Caption(int editor_index);
	String Get_Game_Editor_Caption(int editor_index); 
	int List_Index(int game_index) const;
	int Game_Index(int list_index) const;
	void Invalidate_Order();

	static void Signal_Click_Column(GtkCList *clist,
					gint column,
					gpointer user_data);
	static int Compare(const void *a, const void *b);
};

#endif /* ! GAME_ARCHIVE_EDITOR_HH */

