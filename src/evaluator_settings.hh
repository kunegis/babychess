#ifndef EVALUATOR_SETTINGS_HH
#define EVALUATOR_SETTINGS_HH

#include "pointer.hh"
#include "string.hh"

enum Timing {ti_infinite= 0, ti_time, ti_depth, ti_count};
//enum  {st_never, st_auto, st_always, st_count};

enum {em_off,
      em_analyze,
      em_opponent,
      em_both };

/* flags */ 
enum {evf_no_nice              = 1 << 0,
//      evf_play_move            = 1 << 1,
//      evf_run_single           = 1 << 2,

      evf_count= 1};

class Evaluator_Settings
	:  virtual public Pointed
{
public:
	int mode; /* em_* */ 

	Timing timing;
	int time_s;
	int depth;

//	int start; /* st_* */
	int flags; /* evf_* */ 

	Evaluator_Settings() { Reset(); }

	Evaluator_Settings(Timing new_timing,
			   int new_time_s,
			   int new_flags)
		:  timing(new_timing),
		   time_s(new_time_s),
		   flags(new_flags)
		{ }
	void Reset(); 
};

#endif /* ! EVALUATOR_SETTINGS_HH */
