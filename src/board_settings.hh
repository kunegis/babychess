#ifndef BOARD_SETTINGS_HH
#define BOARD_SETTINGS_HH

#include <stdio.h>

enum 
{
	b_hide_frame        =1<< 0,
	b_tutor             =1<< 1,
	b_no_highlight_last =1<< 2,

	b_count             =    3,
};
	

struct Board_Settings
{
	int flags;
	void Reset() {flags= 0;}
};

bool Read_Stream(FILE *, Board_Settings *);
bool Write_Stream(FILE *, const Board_Settings *);

#endif /* ! BOARD_SETTINGS_HH */ 
