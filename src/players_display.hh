#ifndef PLAYERS_DISPLAY_HH
#define PLAYERS_DISPLAY_HH

#include "clock.hh"
#include "timer.hh"
#include "box.hh"
#include "piece.hh"
#include "label.hh"
#include "chess_info_widget.hh"

class Players_Display
	:  public V_Box,
	   private Timer
{
public:
	bool Open(Pointer <Widget> parent);
	void Set_Turned(bool turned);
	void Set_Name(int color, String name)
		{
			assert (color == cc_w || color == cc_b);
			name_labels[color].Set(name);
		}
	void Set_Clock(int color, int seconds);
	void Set_To_Move(int new_to_move);
	/* value as the member to_move */ 
	void Reset_Clocks(int time /* seconds */ );
	
private:
	H_Box boxes[2];
	Chess_Info_Widget colors[2];
	Label name_labels[2];
	Clock clocks[2];
	V_Box clock_boxes[2]; 
	Label text_clocks[2];

	int to_move;
	/* -1:    no clock ticking
	 * 0, 1:  white/black's clock ticking 
	 */

	void Timer_Alarm();
	
	void Wdg_Close()
		{
			if (Timer::Is())  Timer::Stop(); 
			V_Box::Wdg_Close();
		}

	bool Open_Box(int color);

	static String Format_Clock(int seconds);
};

#endif /* ! PLAYERS_DISPLAY_HH */ 
