#include "gtk_timer.hh"

#include "timer.hh"

#include <gtk/gtkmain.h>

gint Gtk_Timer_Callback(gpointer data)
{
	Timer &timer= *(Timer *)data;
  
	timer.Timer_Alarm(); 

	return TRUE; 
}

bool Timer::Start(int interval_ms)
{
	assert (! Is());

	if (! content.Alloc())
		return false;

	content().id= gtk_timeout_add(interval_ms,
				      & Gtk_Timer_Callback,
				      this);

	return true;
}

void Timer::Stop()
{
	assert (Is());

	gtk_timeout_remove(content().id);

	content.Free(); 
}

Timer::Timer()
{
}

Timer::~Timer()
{
}
