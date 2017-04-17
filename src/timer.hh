#ifndef TIMER_HH
#define TIMER_HH

/* Implementation is in either DEFAULT_TIMER or GTK_TIMER. 
 */

#include "dynamic.hh"
#include "pointer.hh"

class Timer_Content;

class Timer
	:  virtual public Pointed
{
public:

	Dynamic <Timer_Content> content;

	Timer(); 
	~Timer();

	bool Start(int new_interval);
	/* milliseconds */
	void Stop();

	bool Is() const
		{ return content.Is();  }
	virtual void Timer_Alarm()
		{ }
};

#endif /* ! TIMER_HH */
