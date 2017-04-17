#include "busy.hh"

void Busy::Set(int new_state)
{
	assert (Timer::Is() == (state == b_running));

	if (state == new_state)
		return;

	if (state == b_running)
	{
		Timer::Stop();
	}

	state= new_state;
	switch (state)
	{
	default:  assert(0);  break;
	case b_ready:
		(*this)().Set_Color(RGB(0xff, 0xff, 0xff));
		(*this)().Set_Glyph();
		break;
	case b_running:
		Timer::Start(100);
		(*this)().Set_Glyph();
		break;
	case b_paused:
		(*this)().Set_Color(RGB(0xff, 0xff, 0xff));
		(*this)().Set_Glyph(Color_Widget::g_pause);
		break;
	}
}

void Busy::Wdg_Close()
{
	if (Timer::Is())  Timer::Stop();
	Buttoned <Color_Widget> ::Wdg_Close(); 
}


void Busy::Timer_Alarm()
{
	if (! Buttoned <Color_Widget> ::Is())  return;

	++ count;

	int a= count / 16 % 2 ? 16 - count % 16 : count % 16;

	(*this)().Set_Color(RGB(a * 255 / 16, 
				a * 255 / 16, 
				a * 128 / 16));
}

bool Busy::Open(Pointer <Widget> parent,
		String new_msg_clicked)
{
	count= 0;

	if (! Buttoned <Color_Widget> ::Open(parent,
					     new_msg_clicked,
					     RGB(0xff, 0xff, 0xff)))
		return false;

	gtk_button_set_relief(GTK_BUTTON(Get()), GTK_RELIEF_NONE);

	return true;
}
