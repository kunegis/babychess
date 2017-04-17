#include "players_display.hh"

#include "gettext.hh"
#include "format.hh"

bool Players_Display::Open(Pointer <Widget> parent)
{
	if (! V_Box::Open(parent, dialog_unit/2))  return false;

	if (! Open_Box(cc_b))  goto close;
	if (! Open_Box(cc_w))  goto close; 

	to_move= -1;
	
	return true;

 close:
	Close();
	return false;
}

void Players_Display::Set_Turned(bool turned)
{
	boxes[cc_w].Set_Index(turned ? 0 : 1);
	boxes[cc_b].Set_Index(turned ? 1 : 0);
}

String Players_Display::Format_Clock(int seconds)
{
	int sign= 1;
	if (seconds < 0)  
	{
		sign= -1;
		seconds= -seconds; 
	}

	int minutes= seconds / 60;
	int hours= minutes / 60;
	seconds %= 60;

	if (hours)
	{
		return Format("%s%d:%c%c:%c%c", 
			      sign > 0 ? "" : "−",
			      hours, 
			      minutes / 10 + '0', 
			      minutes % 10 + '0',
			      seconds / 10 + '0',
			      seconds % 10 + '0');
	}
	else
	{
		return Format("%s%d:%c%c", 
			      sign > 0 ? "" : "−",
			      minutes, 
			      seconds / 10 + '0',
			      seconds % 10 + '0'); 
	}
}

void Players_Display::Timer_Alarm()
{
	assert (to_move >= 0 && to_move < 2);
	Set_Clock(to_move,
		  - clocks[to_move].Get() - 1);
}

void Players_Display::Set_Clock(int color, int seconds)
{
	assert (color == cc_w || color == cc_b);
	clocks[color].Set(- seconds);
	text_clocks[color].Set(Format_Clock(seconds));
}

void Players_Display::Set_To_Move(int new_to_move)
{
	Verbose_Function();
	Verbose_Int(to_move);
	Verbose_Int(new_to_move);
	assert (new_to_move == cc_w || new_to_move == cc_b
		|| new_to_move == -1);
	if (to_move < 0 && new_to_move >= 0)
	{
		Timer::Start(1000);
	}
	else if (to_move >= 0 && new_to_move < 0 && Timer::Is())
	{
		Timer::Stop(); 
	}
	for (int i= 0;  i < 2;  ++i)
		clocks[i].Set_Blink(new_to_move == i);
	to_move= new_to_move; 
}

bool Players_Display::Open_Box(int color)
{
	if (! boxes[color].Open(this, dialog_unit/2)) return false;

	colors[color].Open(boxes[color], color);
	boxes[color].Set_Next(true);
	if (! name_labels[color].Open(boxes[color],
				     color ? I_("player:Black") : I_("player:White"),
				     0.0,
				     "size=\"x-large\""))
		return false;
	if (! clock_boxes[color].Open(boxes[color]))
		return false;
	if (! clocks[color].Open(clock_boxes[color]))
		return false;
	if (! text_clocks[color].Open(clock_boxes[color],
				      "0:00", 
				      0.5))
		return false;

	return true; 
}

void Players_Display::Reset_Clocks(int time /* seconds */ )
{
	Set_Clock(cc_w, time);
	Set_Clock(cc_b, time);
	Set_To_Move(-1); 
}
