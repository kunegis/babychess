#ifndef BOARD_HH
#define BOARD_HH

/* The graphical chess board */ 

#include "position.hh"
#include "pixmap.hh"
#include "square.hh"
#include "inline.hh"
#include "scaled_board_image.hh"
#include "menu.hh"
#include "board_colors_editor.hh"
#include "preferences.hh"
#include "generator.hh"
#include "bit_board.hh"
#include "drawing_area.hh"
#include "board_settings.hh"
#include "notation.hh"
#include "opening_book.hh"

#include "../dep/strings.hh"

#include <gtk/gtkstatusbar.h>

class Board
	:  public Drawing_Area
{
public:

	struct Open_Info
	{
		Pointer <const Position> position;
		String msg_move;
		String msg_middle;
		String msg_turn;
		String msg_wheel_up, msg_wheel_down; 
		Pointer <const Generator> generator; 
		Pointer <const Opening_Book> opening_book;
		bool turned;
		Menubar &menubar; 
	
		Open_Info(const Position &new_position,
			  String new_msg_move,
			  String new_msg_middle,
			  String new_msg_turn,
			  String new_msg_wheel_up,
			  String new_msg_wheel_down,
			  Pointer <const Generator> new_generator,
			  Pointer <const Opening_Book> new_opening_book,
			  bool new_turned,
			  Menubar &new_menubar)
			:  position(new_position),
			   msg_move(new_msg_move),
			   msg_middle(new_msg_middle),
			   msg_turn(new_msg_turn),
			   msg_wheel_up(new_msg_wheel_up),
			   msg_wheel_down(new_msg_wheel_down),
			   generator(new_generator),
			   opening_book(new_opening_book),
			   turned(new_turned),
			   menubar(new_menubar)
			{ }
	};

	void MR_Receive(String);
	bool Open(Pointer <Widget> parent, 
		  const Open_Info &);
	/* MSG_MOVE is sent after a piece was dragged.  The squares
	 * can be retrieved with Squares().  A message is sent
	 * even when the piece was released outside the board.
	 * In this case Get_Squares() returns DRAG_TO=0. 
	 *
	 * When the middle mouse button is clicked, MSG_MIDDLE is sent
	 * and Get_Squares() returns the square in FROM. 
	 */


	bool Get_Squares(int &from, int &to, guint *state= NULL);
	/* After MSG_MOVE, this functions tells the squares.
	 * Return FALSE if no piece was dragged.  
	 */

	const Board_Colors &Colors() const { return colors(); }
	void Set_Turned(bool new_turned); 
//	bool Turned() const { return preferences->flags & bt_turned; }
	bool Turned() const { return turned; }

	void Set_Last_Move(int square, int square); 
	void Clear_Last_Move() 
		{
			Clear_Last_Highlight();  
			last_a= last_b= 0; 
		}
	int Focus() const { return focus; }
	void Invalidate_Move_Squares(const Move &);
	void Invalidate_Square(int square,  
			       bool interrupt_dragging,
			       bool clear_selection= true);
	void Invalidate_Position();
	/* All of POSITION has changed.   */
	void Caption_Changed() { colors.Caption_Changed();  }

	GtkStatusbar *statusbar;
	/* Can be set to a statusbar which will be used to display
	 * information while dragging.  Text is only pushed there during
	 * dragging, and other information can be displayed there,
	 * e.g. who is to move. 
	 */

	Pointer <const Notation> notation;
	Pointer <const Notation_Settings> notation_settings;

private:

	class B_Colors
	{
	public:
		B_Colors():  have_colors(false) {}
		void Make(const Board_Colors &);
		void Invalidate() {have_colors= false; }
		const GdkColor &operator () (int i, int dark) const
			{
				assert (have_colors);
				return colors[i][dark]; 
			}

	private:

		bool have_colors; 
		GdkColor colors[c_count][2];
		/* Index 0 as in Colors::colors.  Set if HAVE_COLORS.  
		 * Second index:  dark color for the
		 * ring (only for colors that are used for circles)
		 */
	};

	int square_width;
	Scaled_Board_Image scaled_image;
	Pixmap board_image;
	/* The whole board, including the frame if it is visible.  Has
	 * the same size as THIS widget.   */
	std::auto_ptr <Popup_Menu> menu;
	Pointer <const Position> position;
	Pointer <const Generator> generator;
	Pointer <const Opening_Book> opening_book; 
	/* may be NULL */
	String msg_move;
	String msg_middle;
	String msg_turn, msg_wheel_up, msg_wheel_down; 
	bool dragging;
	GdkPoint drag_point;
	GdkPoint pointer_pos;
	/* Position of pointer inside the dragged square.  */
	GdkPoint last_org;
	/* Origin of dragged piece rectangle.   */
	int drag_from;
	int drag_to;
	guint state; /* keys pressed when the key was released */ 
	int dragged_piece;
	Pixmap drag_img;
	/* Same size as BOARD_IMAGE.  Background image when dragging.  */
	Preferences <Board_Colors_Editor> colors;
	Board_Settings settings;
	bool turned;
//	Vector <int> tutor_highlight;
	Bit_Board tutor_high;
	int tutor_no_square;
	/* Squares highlighted in tutor mode.   */
	int last_highlight[2];
	/* what squares are highlighted as "last move" */
	int last_a, last_b; 
	/* the last move (not shown in yellow if not showing the last
	 * move */ 
	int focus, has_focus;
	int selection; 
	B_Colors board_colors; 
	Bit_Board invalid;
	Pointer <Menubar> menubar;
	bool want_clear_selection; 

	static int frame_width;

	void Update_Board_Image_Size();
	/* BOARD_IMAGE must be.  Set SQUARE_WIDTH to -1 before calling to force
	 * new loading.   
	 */

	void Draw_Square(Canvas &,
			 int square,
			 int circle_color,
			 bool empty= false); 
	/* Draw that square.  
	 * color=  0:  normal square
 	 *         other:  index of Colors::colors:  draw a
	 *                 circle in that color
	 *
	 * Don't invalidate that region. 
	 */ 

	void Draw_Actual_Square(int square);
	/* draw square as normal square or selected if the square is
	 * selected. 
	 */

	void Draw_Frame();

	int Point_Square(GdkPoint) const;
	GdkRectangle Square_Rect(int square) const;
	void Interrupt_Dragging();

	void Wdg_Draw(Canvas &);
	void Wdg_Size(GdkPoint);
	void Wdg_Close();
	void Wdg_Get_Size_Request(int &, int &, bool &); 
	bool Wdg_Key_Press(guint);

	bool Is_Inside_Point(GdkPoint p) const;
	void Set_Hide_Frame(bool);
	void Set_Last_Highlight(int square_a, int square_b);
	/* None may be 0. */
	void Clear_Last_Highlight();
	void Update_Last_Highlight();
	bool Open_Menu(); 
	void Set_Menu();
	void Set_Menu_Turned();
	void Set_Menu_Hide_Frame();
	void Set_Menu_Tutor();
	void Set_Menu_No_Highlight_Last();
	//	void Set_Menu_Square_Names();
	void Motion(GdkEventMotion *);
	void Button_Press(GdkEventButton *);
	void Button_Release(GdkEventButton *);
	void Begin_Dragging(GdkPoint p);
	void End_Dragging(GdkPoint p, guint state);
	void Set_Focus(int new_focus); 
	/* SQUARE:  set that square as the focussed square.
	 * 0:  hide the focus
	 * <0:  make the focus visible 
	 */
	void Clear_Focus()
		{
			if (!has_focus)  return;
			invalid.Set(focus);
			GdkRectangle rect= Square_Rect(focus);
			Invalidate(rect);
			has_focus= 0;
		}
	void Clear_Selection()
		{
			if (!selection)  return;
			invalid.Set(selection);
			GdkRectangle rect= Square_Rect(selection);
			Invalidate(rect);
			selection= 0;
		}
	void Set_Selection(int);
	void Save_Settings();
	String Piece_Info(int to_square);
		/* GENERATOR exists and DRAG_FROM
		 * contains the square of the piece.  Return text to be
		 * appended to the statusbar text.  (I.e., prepend a
		 * space when not empty.)  TO_SQUARE is -1 if outside
		 * the board.  
		 */ 

	static const char *Translate(const char *);
	static gint Signal_Motion_Notify(GtkWidget *,
					 GdkEventMotion *,
					 gpointer);
	static gint Signal_Button_Press(GtkWidget *,
					GdkEventButton *,
					gpointer);
	static gint Signal_Button_Release(GtkWidget *,
					  GdkEventButton *,
					  gpointer);
	static gboolean Signal_Focus(GtkWidget *widget,
					GdkEventFocus *event,
					gpointer user_data);	
	static gboolean Signal_Scroll(GtkWidget *,
				      GdkEventScroll *,
				      gpointer);

	static void Draw_Circle(Canvas &canvas, 
				const B_Colors &colors,
				GdkRectangle rect, 
				int circle_color);
	static void Draw_Square_Background(Canvas &canvas,
					   const B_Colors &colors, 
					   GdkRectangle rect,
					   int circle_color,
					   int square_color,
					   int settings_flags,
					   int colors_flags,
					   int square,
					   int focus);
	static void Draw_Exact_Square(Canvas &canvas,
				      Scaled_Board_Image &scaled,
				      const B_Colors &colors,
				      GdkPoint point,
				      int piece,
				      int square_color,
				      int circle_color,
				      int settings_flags,
				      int colors_flags,
				      int square,
				      int focus);
};

#define BOARD_MENU             \
   { "/"N_("_Board"),                     NULL,\
 NULL,         0,                          "<Branch>",     0},  \
   { "/Board/"N_("_Turn Board"),          NULL,\
 MENU_CALLBACK,str_board_turn_board,    "<CheckItem>",  0},  \
   { "/Board/"N_("Hide the _Frame"),      NULL,\
 MENU_CALLBACK,str_board_hide_frame,          "<CheckItem>",  0},  \
   { "/Board/"N_("T_utor Mode"),          NULL,\
 MENU_CALLBACK,str_board_tutor,          "<CheckItem>",  0},  \
   { "/Board/"N_("Don’t Highlight the Last Move"),          NULL,\
 MENU_CALLBACK,str_board_no_highlight_last,         "<CheckItem>",  0},  \
   { "/Board/sep1",                               NULL,\
 NULL,         0,                          "<Separator>",  0},  \
   { "/Board/"N_("_Board Colors"),   NULL,\
 MENU_CALLBACK,str_board_colors,   "<StockItem>",  GTK_STOCK_PREFERENCES}, 

#endif /* ! BOARD_HH */
