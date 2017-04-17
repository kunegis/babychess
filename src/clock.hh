#ifndef CLOCK_HH
#define CLOCK_HH

/* I don't know of a clock in neither GTK+ nor GNOME. 
 */

#include "drawing_area.hh"
#include "timer.hh"

class Clock
	:  public Drawing_Area,
	   private Timer
{
public:

	bool Open(Pointer <Widget> parent)
		{
			time= 0;
			return Drawing_Area::Open(parent);
		}

	void Set(int new_time)
		{
			time= new_time;
			Invalidate();
		}

	int Get() const 
		/* seconds */
		{
			assert(Drawing_Area::Is());
			return time;
		}
	void Set_Blink(bool blink);

private:

	enum {precision= 10};

	int time; 
	/* seconds */
	int phase; /* mod PRECISION */ 

	void Wdg_Get_Size_Request(int & width  ,
				  int &  height  ,
				  bool & resizeable  )
		{
			width= height= 5 * dialog_unit;
			resizeable= false; 
		}

	void Wdg_Draw(Canvas &canvas); 
	void Wdg_Close()
		{
			if (Timer::Is())  Timer::Stop(); 
			Drawing_Area::Wdg_Close();
		}
	void Timer_Alarm();
};

#endif /* ! CLOCK_HH */
