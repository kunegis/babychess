#ifndef DEFAULT_TIMER_HH
#define DEFAULT_TIMER_HH

#include "vector.hh"
#include "pointer.hh"
#include "timer.hh"

class Timer_Content
{
public:

	int index;
	/* index in LIST. 
	 */
	int interval; /* ms */
	struct timeval last_alarm;
};

extern Vector <Pointer <Timer> > timer_list;

#endif /* ! DEFAULT_TIMER_HH */
