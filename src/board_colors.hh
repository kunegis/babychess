#ifndef BOARD_COLORS_HH
#define BOARD_COLORS_HH

#include "rgb.hh"
#include "pointer.hh"

#include <stdio.h>

/* Flags */ 
enum
{
	bt_riffled        = 1 << 0,

	bt_count= 1
};

/* Color indices */ 
enum {c_light,         /* light squares           */
      c_dark,          /* dark_squares            */
      c_last_move,     /* last move circle        */
      c_frame,         /* frame                   */
      c_selection,     /* selected square         */
      c_frame_text,    /* text on the frame       */
      c_text,          /* square text             */
      c_selection_no,  /* tutor:  when no moves are possible */ 

      c_count,

      c_focus_selection,  /* not saved */ 
};


struct Board_Colors
	:  virtual public Pointed
{
	int flags; /* bs_* */ 
	RGB colors[c_count];
	
	void Reset();
	static const char *Default_Conf_Name()
		{
			return "Wood"; 
		}
};

bool Read_Stream(FILE *, Board_Colors *);
bool Write_Stream(FILE *, const Board_Colors *);

#endif /* ! BOARD_COLORS_HH */
