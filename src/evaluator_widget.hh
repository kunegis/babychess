#ifndef EVALUATOR_WIDGET_HH
#define EVALUATOR_WIDGET_HH

#include "box.hh"
#include "label.hh"
#include "framed.hh"
#include "position.hh"
#include "generator.hh"
#include "evaluator_settings_editor.hh"
#include "owned_editor.hh"
#include "evaluator.hh"
#include "format.hh"
#include "busy.hh"
#include "notation.hh"
//#include "menubar.hh"
#include "toolbar.hh"
#include "clock.hh"
#include "engine_editor.hh"
#include "opening_book.hh"
#include "preferences.hh"
#include "piece_widget.hh"
#include "menu.hh"

#include "../dep/strings.hh"

class Evaluator_Widget
	:  public Framed <V_Box> ,
	   public Evaluator
{
public:

	void MR_Receive(String);

	bool Open(Pointer <Widget> parent, 
		  String msg_play,    /* sent when should restart thinking */ 
		  String msg_changed, /* status change (running, finished...) */ 
		  const Position &,
		  const Generator &,
		  const Opening_Book &,
		  const Notation &,
		  const Notation_Settings &,
		  Menubar &menubar,
		  Busy &busy);

	void Reset()
		{
			Reset_Call();
		}
	void Set_Notation(const Notation &);
	void Set_Notation_Settings(const Notation_Settings &);
	void Append_Tool_Buttons(Toolbar &);
	int Get_Best_Move() const
		{
			return Started() ? Evaluator::Best_Move() : move;
		}
	void Start_Or_Play();

private:

	H_Box top_box, top_1_box, top_2_box,
		second_box, wc_box, cpu_box, bottom_box;    
	V_Box top_left_box;

	Label depth_text, best_move_text, score_text;
	Piece_Widget score_piece; 
	Pointer <Busy> busy; 
	Title_Label engine_name;
	Label cpu_share_text, knots_text;
	Label wc_caption, wc_time, wc_speed;
	Label cpu_caption, cpu_time, cpu_speed;
	Label depth_factor_text;
	Label time_text, status_text; 
	Clock clock; /* display CPU time */ 

	Owned_Editor <Evaluator_Settings_Editor> settings_editor;
	Preferences <Engine_Editor> engine;
	String msg_play, msg_changed;
	Pointer <Menubar> menubar;

	enum {button_count= 3};
	GtkWidget *buttons[button_count];
	/* toolbar buttons */

	Notation notation; 
	Notation_Settings notation_settings;
	Pointer <const Opening_Book> opening_book; 
	int move;
	/* Move to play when not starting the engine.  */

	void Wdg_Close();

	void Clear_Info();

	void EC_Stop();
	void EC_Reset();

	void EC_Timer(int, int);
	void EC_Run();
	void EC_Pause();
	void EC_Depth(int depth);
	void EC_Main(int move);
	void EC_Score(int score);
	void EC_Knots(knots_t knots); 
	void EC_Depth_Factor();

	String Get_Time() const;
	void Update_Time(); 
	void Set_Menu();
	void Set_Menu_Reply();
	void Update_Sensitive(); 
	void Update_Engine_Name();
};

/* Note:  The menu item "Play main move" generates the message
 * "play_best", which is by default not sent back to the evaluator
 * widget.  */

#define EVALUATOR_WIDGET_MENU       \
   {"/"N_("_Engine"),                             NULL,     \
    NULL,         0,                         "<Branch>", 0},\
   {"/Engine/"N_("_Start"),                       "<ctrl>9",  \
    MENU_CALLBACK, str_ev_start_or_play,           NULL,  0},  \
   {"/Engine/"N_("_Pause"),                       "<ctrl>0",  \
    MENU_CALLBACK, str_ev_pause,           NULL,  0},  \
   {"/Engine/"N_("R_estart"),                     "<ctrl>3",  \
    MENU_CALLBACK,str_ev_re_start,        NULL,  0},  \
   {"/Engine/"N_("P_lay Main Move"),              "<ctrl>8",  \
    MENU_CALLBACK,str_play_best,       NULL,  0},  \
   {"/Engine/sep1",                                       NULL,     \
    NULL,         0,                         "<Separator>", 0},\
   {"/Engine/"N_("Choose an _Engine"),       NULL,         \
    MENU_CALLBACK,  str_ev_engine,   "<StockItem>", GTK_STOCK_PREFERENCES},\
   {"/Engine/"N_("_Preferences"),          NULL,     \
    MENU_CALLBACK,str_ev_settings,        "<StockItem>",GTK_STOCK_PREFERENCES},

#if 0
   {"/Engine/" N_("_Never Start Engine"),          NULL,     \
    MENU_CALLBACK,str_ev_reply_0,         "<RadioItem>", 0},\
   {"/Engine/"N_("_Start Engine After Each User Move"),NULL,     \
    MENU_CALLBACK,str_ev_reply_1,         "/Engine/Never Start Engine", 0},\
   {"/Engine/"N_("_Always Start Engine"),         NULL,     \
    MENU_CALLBACK,str_ev_reply_2,         "/Engine/Never Start Engine", 0},\
   {"/Engine/sep4",                                       NULL,     \
    NULL,         0,                         "<Separator>", 0},
#endif /* 0 */ 


#endif /* ! EVALUATOR_WIDGET_HH */
