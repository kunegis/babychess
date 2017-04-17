#include "piece_widget.hh"

void Piece_Widget::Wdg_Draw(Canvas &canvas)
{
	if (piece & pi_empty)  return; 

	if (! image.Is())
	{
		if (! image.Load(size/*, true*/))
			return;
	}

	GdkPoint own_size= Size();

	image.Draw(canvas,
		   (GdkPoint){(own_size.x-size)/2,
				      (own_size.y-size)/2}, 
		   piece & pi_type,
		   Piece_Color(piece));
}

void Piece_Widget::Set_Piece(int new_piece)
{
	piece= new_piece;
	Invalidate(); 
}

bool Piece_Widget::Open(Pointer <Widget> parent,
			int new_piece,
			int new_size)
{
	piece= new_piece;
	size=  new_size; 
	return Drawing_Area::Open(parent);
}
