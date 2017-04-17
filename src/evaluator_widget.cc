#include "evaluator_widget.hh"

#include "error.hh"
#include "format.hh"
#include "inform.hh"
#include "fen.hh"
#include "parse_move.hh"
#include "time.hh"
#include "format_time.hh"
#include "verbose.hh"
#include "score.hh"
#include "inform.hh"
#include "gettext.hh"
#include "printf_sep.hh"
#include "version_number.hh"
#include "format_move_name.hh"

#include <locale.h>

bool Evaluator_Widget::Open(Pointer <Widget> parent,
			    String new_msg_play,
			    String new_msg_changed,
			    const Position &new_position,
			    const Generator &new_generator,
			    const Opening_Book &new_opening_book,
			    const Notation &new_notation,
			    const Notation_Settings &new_notation_settings,
			    Menubar &new_menubar,
			    Busy &new_busy)
{
	engine.Open(*this, "ev_engine_edited");
	engine_choice= engine(); 
	if (! Framed <V_Box>::Open(parent,
				      Framed <V_Box> ::Open_Info
				      (V_Box::Open_Info(dialog_unit/2))))
		return false;

	if (! Evaluator::Open(new_position, new_generator)) 
		goto close;

  	msg_play=       new_msg_play;
	msg_changed=    new_msg_changed;
	notation=       new_notation; 
	notation_settings= new_notation_settings;
	opening_book=   new_opening_book; 

	if (! engine_name.Open ((*this)(), ""))  goto close;	
	if (! top_box.Open((*this)(), dialog_unit))                          goto close;
	top_box.Set_Next(true); 
	if (! top_left_box.Open(top_box, dialog_unit))                      goto close; 
	if (! top_1_box.Open(top_left_box))                    goto close; 
	if (! depth_text.Open(top_1_box, "", 0.0, "size=\"xx-large\""))     goto close;
	top_1_box.Set_Next(true);
	if (! best_move_text.Open(top_1_box, "    ", 0.0, "size=\"large\""))         goto close;
	if (! score_text.Open(top_1_box, "", 0.0, "size=\"large\""))
		goto close;
	if (! score_piece.Open(top_1_box, pi_empty, dialog_unit*5/3))  goto close;
	if (! top_2_box.Open(top_left_box, dialog_unit))                    goto close; 
	if (! time_text.Open(top_2_box, Get_Time()))                             goto close; 
	if (! status_text.Open(top_2_box))                                       goto close;
	if (! clock.Open(top_box))                                               goto close; 

	if (! second_box.Open((*this)(), dialog_unit)) goto close;
	second_box.Set_Next(true);
	if (! cpu_share_text.Open(second_box))   goto close;
	if (! depth_factor_text.Open(second_box))         goto close; 
	if (! knots_text.Open(second_box))       goto close; 

	if (! wc_box.Open((*this)(), dialog_unit))     goto close;
	if (! wc_caption.Open(wc_box, _("Realtime:")))   goto close;
	wc_box.Set_Next(true);
	if (! wc_time.Open(wc_box))              goto close;
	if (! wc_speed.Open(wc_box))             goto close;

	if (! cpu_box.Open((*this)(), dialog_unit))    goto close;
	if (! cpu_caption.Open(cpu_box,
			       _("CPU:")))    goto close;
	cpu_box.Set_Next(true);
	if (! cpu_time.Open(cpu_box))            goto close;
	if (! cpu_speed.Open(cpu_box))           goto close;

	if (! bottom_box.Open((*this)(), dialog_unit)) goto close;
	bottom_box.Set_Next(true);

	busy= new_busy; 

	menubar= new_menubar;
	Set_Menu(); 
	Update_Sensitive();
	Update_Engine_Name();

	return true;

 close:

	Widget::Close();
	return false;
}

void Evaluator_Widget::MR_Receive(String message)
{
	if (message == "ev_start")
	{
		if (Started())
		{
			if (Exists() && Paused())
			{
				Pause_Continue();
				busy->Set(b_running);
				Update_Sensitive();
			}
		}
		else
		{
			if (! Start())
			{
				Inform_Error(_("Cannot start the engine"),
					     this); 
				settings.mode= em_off; 
//				settings.start= st_never; 
				Set_Menu_Reply(); 
			}
			else
			{
				busy->Set(b_running);
				Update_Sensitive();
			}
		}
	}
	else if (message == "ev_start_or_play")
	{
		/* There's only one move--play it */ 
		if (*Get_Generator() == 1 &&
		    settings.mode != em_analyze)
//		if (!( (settings.flags & evf_run_single)  ||
//		       *Get_Generator() > 1))
		{
			if (!*Get_Generator())  return;
			move= 0;
			depth_text.Set(Format("[%s]  ", _("single move")));
			status_text.Set(_("finished"));
			best_move_text.Set(Format_Move_Name
					   (Get_Generator().Get_Move_Name(0),
					    notation,
					    notation_settings)
					   +"    ");
//			if (settings.flags & evf_play_move)
//			{
				Parent_Receive_Message(msg_play);
//			}
		}

		/* We're in the opening book */ 
		else if (opening_book && opening_book->Has_Moves())
		{
			move_t small_move;
			bool cro= opening_book->Choose_Random_Opening(small_move);
			assert (cro);
			move= Get_Generator().Small_Move(small_move); 
			depth_text.Set(Format("[%s]  ", _("book"))); 
			status_text.Set(_("finished"));
			best_move_text.Set
				(Format_Move_Name
				 (Get_Generator().Get_Move_Name(move),
				  notation,
				  notation_settings)
					+"    ");
			Parent_Receive_Message(msg_changed);
			if (settings.mode != em_analyze)
//			if (settings.flags & evf_play_move)
				Parent_Receive_Message(msg_play);
		}
	
		/* start the engine */ 
		else
		{
			MR_Receive("ev_start"); 
		}
	}
	else if (message == "ev_pause")
	{
		if (Started() && Exists()) 
		{
			Pause_Continue();
			busy->Set(Paused() ? b_paused : b_running);
			Update_Sensitive();
		}
	}

	else if (message == "ev_re_start")
	{
		Re_Start();
		busy->Set(b_running);
		Update_Sensitive(); 
	}
	else if (message == "ev_settings")
	{
		settings_editor.Open ("ev_settings_edited", 
				      *this, settings, this); 
	}
#if 0
	else if (!memcmp(message(), "ev_reply_", 9))
	{
		int index;
		if (1!=sscanf(message()+9, "%d", &index))  return;
		if (index < 0 || index >= st_count)  return;
		settings.start=  (index);  
		Set_Menu_Reply(); 

		/* If setting to always, start engine now. */
		if (index == 2)
		{
			if (! Started())
			{
				Start_Or_Play();
			}
		}
	}
#endif /* 0 */
	else if (message == "ev_settings_edited")
	{
		Evaluator_Settings es= settings_editor.Value();
		settings_editor.Have_Read();
	
		settings= es;
		Update_Time();

		/* Start engine if settings were set to always
		 * analyze.  */
		if (settings.mode == em_analyze && ! Started())
			Start_Or_Play();

		Set_Menu(); 
	}
	else if (message == "ev_busy")
	{
		if (! Started())        Start_Or_Play();
		else if (! Exists())    MR_Receive("ev_restart");
		else                    MR_Receive("ev_pause"); 
	}
	else if (message == "ev_engine")
	{
		engine.Edit(); 
	}

	else if (message == "ev_engine_edited")
	{
		Reset();
		engine_choice= engine(); 
		Update_Engine_Name(); 
	}

	else 
		Framed <V_Box> ::MR_Receive(message);
}

void Evaluator_Widget::Wdg_Close()
{

	if (IO_Child_Process::Is())  Reset();
	if (Timer::Is())             Timer::Stop();
 	Evaluator::Close();
	settings_editor.Close();
	busy= NULL; 
	opening_book= NULL; 
	engine.Close();

	Framed <V_Box> ::Wdg_Close();
}

void Evaluator_Widget::Clear_Info()
{
	status_text.Set("");
	depth_text.Set("");
	best_move_text.Set("    ");
	score_text.Set("");
	score_piece.Set_Piece(pi_empty);
	cpu_share_text.Set("");
	knots_text.Set("");
	wc_time.Set("");
	wc_speed.Set("");
	cpu_time.Set("");
	clock.Set(0);
	cpu_speed.Set("");
	depth_factor_text.Set("");
	busy->Set(b_ready);
}

void Evaluator_Widget::EC_Timer(int date_ms,  int time_ms)
{
	/* Display the date only with second precision  */

	int date_s= date_ms / 1000;

	wc_time.Set(Format_Time(date_s, -1, date_ms % 1000)); 

	date_ms= std::max(date_ms, 1); 
//	date_ms >?= 1;
		/* to avoid division by 0 */ 

	cpu_time.Set(Format_Time(time_ms / 1000, 3, time_ms % 1000));
	clock.Set(time_ms / 1000);
	cpu_share_text.Set(Format("%d%s%c %%",
				   time_ms * 100 / date_ms,
				   localeconv()->decimal_point,
				   '0' + time_ms * 1000 / date_ms % 10));
}

void Evaluator_Widget::EC_Knots(knots_t knots)
{
	static const char *kt= NULL;
	if (kt == NULL)  kt= _("KT");
	knots_text.Set(Format("%"PRINTF_SEP PRIknots" %s", knots, kt));
	int date_ms= Evaluator::Age_ms(); 
	wc_speed.Set(Format("%"PRINTF_SEP PRIknots" %s/s", knots * 1000 / std::max(date_ms, 1), kt));

	Evaluator::Update_CPU_Age();
	cpu_speed.Set(Format("%"PRINTF_SEP PRIknots" %s/s", 
			     knots * 1000 / std::max(Evaluator::CPU_Age_ms(), 1), kt));
}

void Evaluator_Widget::EC_Depth_Factor()
{
	depth_factor_text.Set
		(Format("(↑ %.2f)", 
			Evaluator::Depth_Factor()));
}

void Evaluator_Widget::EC_Stop()
{
	status_text.Set(_("finished"));
	busy->Set(b_ready);
	if (settings.mode != em_analyze && settings.mode != em_off)
//	if (settings.flags & evf_play_move)
		Parent_Receive_Message(msg_play);
	Parent_Receive_Message(msg_changed);
	Update_Sensitive();
}

void Evaluator_Widget::EC_Reset()
{
	Clear_Info();
	Update_Sensitive();
}

void Evaluator_Widget::EC_Score(int score)
{
	score_text.Set (Is_Win_Lose(score)
			? Format_Score (score, 2)
			: Format("%s %s", 
				 Format_Score (score, 2)(),
				 I_("multiplication sign:×")));
	score_piece.Set_Piece(Is_Win_Lose(score) ? pi_empty : 
			      Piece(Get_Position().To_Move(), pi_P));
}

void Evaluator_Widget::EC_Run()
{
	status_text.Set(_("running"));
	score_piece.Set_Piece(Piece(Get_Position().To_Move(), pi_P)); 
	Parent_Receive_Message(msg_changed);
	Update_Sensitive();
}

void Evaluator_Widget::EC_Pause()
{
	status_text.Set(_(IO_Child_Process::Paused() 
			  ? N_("pause") : N_("running")));
	Parent_Receive_Message(msg_changed);
	Update_Sensitive();
}

void Evaluator_Widget::EC_Depth(int depth)
{
	String formatted;
	if (depth > 20)
	{
		formatted= Format("(%d)  ", depth);
	}
	else
	{
		char text[]= "①  ";
		assert (depth >= 1);
		text[2] += (depth - 1);
		formatted= text; 
		formatted.Isolate(); 
	}
	depth_text.Set(formatted);
}

void Evaluator_Widget::EC_Main(int move)
{
	best_move_text.Set(Format_Move_Name
			   (Get_Generator().Get_Move_Name(move),
			    notation,
			    notation_settings)+"    ");
}

String Evaluator_Widget::Get_Time() const
{
	if (settings.timing == ti_infinite)
		return Format("[%s]", I_("infinite:inf"));
	else
		return Format("[%ds]", settings.time_s); 
}

void Evaluator_Widget::Update_Time()
{
	time_text.Set(Get_Time());
}

void Evaluator_Widget::Set_Notation(const Notation &new_notation)
{
	notation= new_notation; 
	if (best_move_text.Text() == "    ")  return;
	EC_Main(Evaluator::Best_Move()); 
}

void Evaluator_Widget::Set_Menu()
{
	Set_Menu_Reply();
}

void Evaluator_Widget::Set_Menu_Reply()
{
#if 0
	menubar->Set_Active(settings.start == st_never
			    ? "/Engine/Never Start Engine" : 
			     settings.start == st_auto
			     ? "/Engine/Start Engine After Each User Move"
			     : "/Engine/Always start engine",
			     true);
#endif /* 0 */ 
}

void Evaluator_Widget::Append_Tool_Buttons(Toolbar &toolbar)
{
	buttons[0]= toolbar.Append_Image(_("Start"), "ev_start_or_play", _("Start the engine"),
					 "start");
	buttons[1]= toolbar.Append_Image(_("Pause"),  "ev_pause",  _("Pause the engine"),
					 "pause");
	buttons[2]= toolbar.Append_Image(_("Play"),   "play_best", 
					 _("Play the engine’s current best move"),
					 "play-best"); 
}

void Evaluator_Widget::Update_Sensitive()
{
	bool start= !Evaluator::Started() || 
		(Evaluator::Exists() &&
		 Evaluator::Paused());
	menubar->Set_Sensitive("/Engine/Start", start);
	gtk_widget_set_sensitive(buttons[0], start);

	bool pause= Evaluator::Started() && Evaluator::Exists();
	menubar->Set_Sensitive("/Engine/Pause", pause);
	gtk_widget_set_sensitive(buttons[1], pause);

	menubar->Set_Sensitive("/Engine/Restart", Evaluator::Started());

	menubar->Set_Sensitive("/Engine/Play Main Move", Evaluator::Started()); 
	gtk_widget_set_sensitive(buttons[2], Evaluator::Started()); 
}

void Evaluator_Widget::Update_Engine_Name()
{
	if (engine_choice.type == et_babychess)
	{
		engine_name.Set
			(Format("BabyChess "VERSION_NUMBER" (%s)",
				Version_Date()));
	}
	else
		engine_name.Set
			(engine_choice.program); 
	
}

void Evaluator_Widget::Start_Or_Play()
{
	MR_Receive("ev_start_or_play");
}

