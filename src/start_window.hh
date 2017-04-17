#ifndef START_WINDOW_HH
#define START_WINDOW_HH

#include "box.hh"
#include "text_button.hh"
#include "process_open_info.hh" 
#include "bevel.hh"
#include "window.hh"

#include <stdio.h>

class Start_Window
	:  public Window
{
public:
	typedef Process_Open_Info Open_Info; 

	bool Open(Open_Info);
	
private:
	class Info
	{
	public: 
		const char *text, *mode;
	};
	enum {size= 5};

	static Info info[size];

	H_Box main_box; 
	Text_Button buttons[size], about_button, close_button;
	V_Box button_box;
	Bevel bevel, first_bevel; 

	void MR_Receive(String); 
};

#endif /* ! START_WINDOW_HH */ 
