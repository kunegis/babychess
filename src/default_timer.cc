#include "default_timer.hh"

#include "time.hh"

Vector <Pointer <Timer> > timer_list;

bool Timer::Start(int new_interval)
{
	assert (! Is());

	if (! content.Alloc())
		return false;

	for (int i= 0;  i < *timer_list && content().index == -1;  ++i)
		if (!timer_list[i])
		{
			content().index= i;
		}

	if (! timer_list.Set(*timer_list + 1))
		return false;
	content().index= *timer_list - 1; 
	
	timer_list[content().index]= this;

	content().interval= new_interval;
	content().last_alarm= Now();

	return true;
}

void Timer::Stop()
{
	if (! Is())
		return; 

	timer_list[content().index]= NULL;

	int new_size= *timer_list;
	while (new_size != 0 && !timer_list[new_size - 1])
		-- new_size;
	timer_list= new_size; 

	content.Free();
}

Timer::~Timer() 
{
	assert (! Is()); 
}

Timer::Timer()
{
}
