#ifndef BUSY_HH
#define BUSY_HH

/* An activity indicator */

#include "timer.hh"
#include "color_widget.hh"
#include "buttoned.hh"

enum {b_ready,    /* white */ 
      b_running,  /* blinking */ 
      b_paused};  /* pause symbol */

class Busy
	:  public Buttoned <Color_Widget> ,
	   public Timer
{
public:

	bool Open(Pointer <Widget> parent,
		  String new_msg_clicked);

	void Set(int state);
	int Get_State() const  {  return state;   }

private:

	int count;
	int state; /* b_* */

	void Wdg_Close();
	void Timer_Alarm();
};

#endif /* ! BUSY_HH */
