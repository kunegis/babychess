#ifndef CHESS_INFO_WIDGET_HH
#define CHESS_INFO_WIDGET_HH

/* A black or white square */ 

#include "piece.hh"
#include "drawing_area.hh"

class Chess_Info_Widget
	:  public Drawing_Area
{
public:

	enum {t_empty= 2, t_down, t_up};

	bool Open(Pointer <Widget> parent,
		  int new_chess_color= cc_w);
	void Set(int); 

private:
	int chess_color;
	GdkColor color; /* for up/down */
	bool have_color;

	void Wdg_Get_Size_Request(int & /* width */ ,
				  int & /* height */ ,
				  bool & /* resizeable */ );
	void Wdg_Draw(Canvas &);
};

#endif /* ! CHESS_INFO_WIDGET_HH */ 
