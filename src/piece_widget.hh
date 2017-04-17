#ifndef PIECE_WIDGET_HH
#define PIECE_WIDGET_HH

#include "scaled_board_image.hh"
#include "piece.hh"
#include "drawing_area.hh"

class Piece_Widget
	:  public Drawing_Area
{
public:
	bool Open(Pointer <Widget> parent,
		  int new_piece,
		  int new_size= 5*dialog_unit);

	void Set_Piece(int new_piece);

private:
	int piece;
	int size;
	Scaled_Board_Image image;

	void Wdg_Draw(Canvas &canvas);

	void Wdg_Get_Size_Request(int &w, int &h, bool &)
		{ w= h= size;  }
};

#endif /* ! PIECE_WIDGET_HH */ 
