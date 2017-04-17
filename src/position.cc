#include "position.hh"

#include "moves.hh"
#include "error.hh"
#include "format.hh"
#include "verbose.hh"
#include "gettext.hh"

void Position::Init_Outside()
{
	/* above & below */
	for (int i= 0;  i < 2;  ++i)
	{
		for (int j= i * 100;  j < i * 100 + 20; ++j)
			pieces[j]= pi_outside;
	}

	/* left & right */
	for (int i= 20; i < 100;  i += 10)
	{
		pieces[i]= pi_outside; 
		pieces[i + 9]= pi_outside; 
	}
}

void Position::Reset()
{
	/* enter legal mode */ 
	legal= true;

	/* empty squares */
	for (int rank= sq_3;  rank <= sq_6;  ++rank)
		memset(pieces.pieces+Make_Square(sq_a, rank), pi_empty, 8);

	/* pawns */
	memset(pieces.pieces+sq_a2, pi_wP, 8);
	memset(pieces.pieces+sq_a7, pi_bP, 8);

	/* First rows */
	int first_row[]= {pi_R, pi_N, pi_B, pi_Q, pi_K, pi_B, pi_N, pi_R};

	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		pieces[Make_Square(file, sq_1)]= Piece(cc_w, first_row[file - sq_a]);
		pieces[Make_Square(file, sq_8)]= Piece(cc_b, first_row[file - sq_a]);
	}

	/* material */
	for (int cc= 0;  cc < 2;  ++cc)
	{
		material(cc, pi_P)= 8;
		material(cc, pi_N)= 2;
		material(cc, pi_B)= 2;
		material(cc, pi_R)= 2;
		material(cc, pi_Q)= 1;
		material(cc, pi_K)= 1;
	}

	/* other */
	memset(moved, 0, sizeof (moved));
	to_move= cc_w;
	ep_poss= 0;
	king_squares[cc_w]= sq_e1;
	king_squares[cc_b]= sq_e8;
	previous_move_count= 0;
	halfmove_clock= 0;
	in_check= false;
}

void Position::Empty()
{
	legal= false;

	/** pieces */

	for (int rank= sq_1;  rank <= sq_8;  ++rank)
	{
		memset(pieces.pieces + (rank*10 + 21), pi_empty, 8);
	}

	/** rest */
	material.Clear();
	to_move= cc_w;
	previous_move_count= 0; /* == to_move */ 
	halfmove_clock= 0;
}

void Position::operator << (const Move &move)
{
	assert (legal); 
	assert (pieces[move.from] & pi_color);
	assert (move.ep || pieces[move.to] == move.captured);
	assert (move.to > 0 && move.to < 120);

	/* Move the piece */

	pieces[move.to]= pieces[move.from];
	pieces[move.from]= pi_empty;

	/* Castling */

	if (move.castling)
	{
		switch (move.castling)
		{
		case wO_O:
			pieces[sq_h1]= pi_empty;
			pieces[sq_f1]= Piece(cc_w, pi_R);
			break;
  
		case wO_O_O:
			pieces[sq_a1]= pi_empty;
			pieces[sq_d1]= Piece(cc_w, pi_R);
			break;
  
		case bO_O:
			pieces[sq_h8]= pi_empty;
			pieces[sq_f8]= Piece(cc_b, pi_R);
			break;

		case bO_O_O:
			pieces[sq_a8]= pi_empty;
			pieces[sq_d8]= Piece(cc_b, pi_R);
			break;

		default:
			assert(0);
		}
	}

	/* Change side to move */
	
	to_move ^= 1;

	/* Update MOVED */ 

	if (Is_Move_Square(move.from))
		++ moved[Square_Move_Index(move.from)];

	if (Is_Move_Square(move.to))
		++ moved[Square_Move_Index(move.to)];

	/* MATERIAL & HALFMOVE_CLOCK */

	if (move.captured & pi_color)
	{
		-- material(to_move, move.captured & pi_type);
		halfmove_clock= 0;
	}
	else
		++ halfmove_clock;

	if ((pieces[move.to] & pi_type) == pi_P)
		halfmove_clock= 0;

	/* Promotion */ 

	if (move.promotion & pi_color)
	{
		pieces[move.to]= move.promotion;
		-- material(1^to_move, pi_P);
		++ material(1^to_move, move.promotion & pi_type);
	}

	/* En passant capture */

	else if (move.ep)
		pieces[move.ep]= pi_empty/*Piece(false)*/;

	/* New en passant square for double pawn advances */ 

	if (pieces[move.to] == Piece(cc_w, pi_P) && 
	    Rank(move.from) == sq_2       &&
	    Rank(move.to) == sq_4)
		ep_poss= move.to;
	else if (pieces[move.to] == Piece(cc_b, pi_P) &&
		 Rank(move.from) == sq_7              &&
		 Rank(move.to) == sq_5)
		ep_poss= move.to;
	else
		ep_poss= 0;

	/* king square */

	if ((pieces[move.to] & pi_istype) == pi_K)
	{
		king_squares[Piece_Color(pieces[move.to])]= move.to; 
	}

	/* in_check */
	in_check= false;
	if (Attacks(move.to)) /* pieces gives check */ 
		in_check= true;
	else if (Attacked_From(move.from))  /* discovery check */ 
		in_check= true;
	else if (move.ep && Attacked_From(move.ep)) 
		/* discovery check by e.p. captured pawn */
		in_check= true;
	else if (move.castling) /* check by castling rook */ 
	{
		static const int rook_squares[4]= {sq_f1, sq_d1, sq_f8, sq_d8};
		if (Attacks(rook_squares[move.castling-4]))
			in_check= true;
	}

	/* previous moves */
	++ previous_move_count;

	/* material check */ 
	assert (material.Check()); 
}

void Position::Light_Play(const Move &move)
{
	/* Content mostly copied from operator << (). 
	 */

	pieces[move.to]= pieces[move.from];
	pieces[move.from]= pi_empty;
	if (move.castling)
	{
		switch (move.castling)
		{
		case wO_O:
			pieces[sq_h1]= pi_empty;
			pieces[sq_f1]= Piece(cc_w, pi_R);
			break;
  
		case wO_O_O:
			pieces[sq_a1]= pi_empty;
			pieces[sq_d1]= Piece(cc_w, pi_R);
			break;
  
		case bO_O:
			pieces[sq_h8]= pi_empty;
			pieces[sq_f8]= Piece(cc_b, pi_R);
			break;

		case bO_O_O:
			pieces[sq_a8]= pi_empty;
			pieces[sq_d8]= Piece(cc_b, pi_R);
			break;

		default:
			assert(0);
		}
	}
	else if (move.ep)
		pieces[move.ep]= pi_empty;
	else if (move.promotion & pi_color)
	{
		pieces[move.to]= move.promotion;
	}
}

void Position::operator >> (const Move &move)
{
	assert(legal);
	assert(pieces[move.to] & pi_color);

	/* pieces */ 

	pieces[move.from]= pieces[move.to];
	pieces[move.to]= move.captured;

	/* TO_MOVE */ 

	to_move ^= 1;

	/* MOVED */ 

	if (Is_Move_Square(move.from))
		-- moved[Square_Move_Index(move.from)];
	if (Is_Move_Square(move.to))
		-- moved[Square_Move_Index(move.to)];

	/* old EP_POSS */ 

	ep_poss= move.ep_poss;

	/* MATERIAL */

	if (move.captured & pi_color)
	{
		++ material(1^to_move, move.captured & pi_type);
	}

	/* halfmove clock */

	halfmove_clock= move.halfmove_clock; 
	assert (halfmove_clock >= 0);
	
	/* castling */ 

	if (move.castling != 0)
	{
		switch (move.castling)
		{
		case wO_O:
			pieces[sq_f1]= pi_empty;
			pieces[sq_h1]= Piece(cc_w, pi_R);
			break;

		case wO_O_O:
			pieces[sq_d1]= pi_empty;
			pieces[sq_a1]= Piece(cc_w, pi_R);
			break;

		case bO_O:
			pieces[sq_f8]= pi_empty;
			pieces[sq_h8]= Piece(cc_b, pi_R);
			break;

		case bO_O_O:
			pieces[sq_d8]= pi_empty;
			pieces[sq_a8]= Piece(cc_b, pi_R);
			break;

		default:
			assert (0);
		}
	}

	/* Take back en passant move */

	else if (move.ep)
	{
		assert(ep_poss == move.ep);
		pieces[move.ep]= move.captured;
		pieces[move.to]= pi_empty;
	}

	/* Take back promotion */ 
	
	else if (move.promotion & pi_color)
	{
		pieces[move.from]= Piece(to_move, pi_P);
		++ material(to_move, pi_P);
		-- material(to_move, move.promotion & pi_type);
	}

	/* king square */ 

	if ((pieces[move.from] & pi_istype) == pi_K)
		king_squares[Piece_Color(pieces[move.from])]= move.from;

	/* previous moves */ 

	assert (previous_move_count > 0);
	--previous_move_count;

	assert (material.Check());

	/* in_check */
	in_check= move.check; 
}

void Position::Light_Take_Back(const Move &move)
{
	/* Content mostly copied from operator >> ().  */

	pieces[move.from]= pieces[move.to];
	pieces[move.to]= move.captured;
	if (move.castling != 0)
	{
		switch (move.castling)
		{
		case wO_O:
			pieces[sq_f1]= pi_empty;
			pieces[sq_h1]= Piece(cc_w, pi_R);
			break;

		case wO_O_O:
			pieces[sq_d1]= pi_empty;
			pieces[sq_a1]= Piece(cc_w, pi_R);
			break;

		case bO_O:
			pieces[sq_f8]= pi_empty;
			pieces[sq_h8]= Piece(cc_b, pi_R);
			break;

		case bO_O_O:
			pieces[sq_d8]= pi_empty;
			pieces[sq_a8]= Piece(cc_b, pi_R);
			break;

		default:
			assert (0);
		}
	}

	/* Take back en passant move */

	else if (move.ep)
	{
		assert(ep_poss == move.ep);
		pieces[move.ep]= move.captured;
		pieces[move.to]= pi_empty;
	}

	/* Take back promotion */ 
	
	else if (move.promotion & pi_color)
	{
		pieces[move.from]= Piece(to_move, pi_P);
	}

}

int Position::Square_Attacked(int square, 
			  int color,
			  bool multiple) const 
{
	int max= multiple ? 2 : 1;
	int result= 0;

#define FOUND    ++result;  if (result == max)  return result; 

	/* king */
	for (int offs= 0;  offs < 8;  ++ offs)
		if (pieces[square + moves_offset[offs]] == Piece(1^color, pi_K))  {FOUND}

	/* pawn */
	if (color == cc_b)
	{
		if (pieces[square -  9] == Piece(cc_w, pi_P))  {FOUND}
		if (pieces[square - 11] == Piece(cc_w, pi_P))  {FOUND}
	}
	else
	{
		if (pieces[square +  9] == Piece(cc_b, pi_P)) {FOUND}
		if (pieces[square + 11] == Piece(cc_b, pi_P)) {FOUND}
	}

	/* files and ranks */
	for (int offs= 0;  offs < 4;  ++ offs)
	{
		int i= 1;

		while (pieces[square + i * moves_offset[offs]] == pi_empty)
			++i;

		if (pieces[square + i * moves_offset[offs]] == Piece(1^color, pi_Q)   ||
		    pieces[square + i * moves_offset[offs]] == Piece(1^color, pi_R))
		{FOUND}
	}

	/* diagonals */
	for (int offs= 4;  offs < 8;  ++ offs)
	{
		int i= 1;
      
		while (pieces[square + i * moves_offset[offs]] == pi_empty)
			++i;

		if (pieces[square + i * moves_offset[offs]] == Piece(1^color, pi_Q) ||
		    pieces[square + i * moves_offset[offs]] == Piece(1^color, pi_B))
		{FOUND}
	}

	/* knight */
	for (int offs= 8;  offs < 16;  ++ offs)
		if (pieces[square + moves_offset[offs]] == Piece(1^color, pi_N)) 
		{FOUND}

#undef FOUND

	return result; 
}

void Position::Update_Material()
{
	assert (!legal);

	Set_Material(material);
}

void Position::Set_Material(Material &material)
{
	for (int cc= 0;  cc < 2;  ++cc)
		for (int pt= 0;  pt < 6;  ++pt)
			material(cc, pt)= 0;

	for (int file= sq_a;  file <= sq_h;  ++file)
		for (int rank= sq_1;  rank <= sq_8;  ++rank)
		{
			int piece= pieces[Make_Square(file, rank)];
			if (piece & pi_color)
				++ material(Piece_Color(piece), piece & pi_type);
		}
}

void Position::Transfer_Piece(int from, int to)
{
	assert (!legal);
	assert (Is_Inside_Square(from));
	assert (Is_Inside_Square(to));

	int to_piece= pieces[to];
	if (to_piece & pi_color)
		-- material(to_piece);
	pieces[to]= pieces[from];
	pieces[from]= pi_empty;
	if ((pieces[to] & pi_istype) == pi_K)
		king_squares[Piece_Color(pieces[to])]= to; 
}

void Position::Change_To_Move()
{
	assert (! legal);
	if (to_move != cc_w)
	{
		to_move= cc_w;
		-- previous_move_count;
	}
	else
	{
		to_move= cc_b;
		++ previous_move_count;
	}
}

void Position::Insert_Piece(int square, int piece)
{
	assert (!legal);
	assert (Is_Inside_Square(square));
	assert (piece & pi_color);
	assert (pieces[square] == pi_empty);

	pieces[square]= piece;
	++ material(piece);
	if ((piece & pi_type) == pi_K)
		king_squares[Piece_Color(piece)]= square;
}

bool Position::Set_Legal()
{
	assert (!legal);

	/* check material */
#ifndef NDEBUG
	Material real_material;
	Set_Material(real_material);
	assert (material == real_material);
	assert (material.Check()); 
#endif	/* ! NDEBUG */ 

	/* no pawns on first row */
	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		{
			int piece= pieces[Make_Square(file, sq_1)];
			if ((piece & pi_istype) == pi_P)
			{
				String square= Format("%c1", file+'a');
				Error::Set(Format(_("Pawn on %s"), square()));
				return false;
			}
		}
		{
			int piece= pieces[Make_Square(file, sq_8)];
			if ((piece & pi_istype) == pi_P)
			{
				String square= Format("%c8", file+'a');
				Error::Set(Format(_("Pawn on %s"), square()));
				return false;
			}
		}
	}

	/* no multiple kings */
	int count[2]= {0, 0};
	for (int file= sq_a;  file <= sq_h;  ++file)
		for (int rank= sq_1;  rank <= sq_8;  ++rank)
		{
			int piece= pieces[Make_Square(file, rank)];
			if ((piece & pi_istype) == pi_K)
			{
				++ count[Piece_Color(piece)]; 
				king_squares[Piece_Color(piece)]= Make_Square(file, rank); 
			}
		}
	if (count[0] < 1)
	{
		Error::Set(_("No white king"));
		return false;
	}
	if (count[0] > 1)
	{
		Error::Set(_("Multiple white kings"));
		return false;
	}
	if (count[1] < 1)
	{
		Error::Set(_("No black king"));
		return false;
	}
	if (count[1] > 1)
	{
		Error::Set(_("Multiple black kings"));
		return false;
	}
	
	/* check king position */

	assert (pieces[King_Square(cc_w)] == pi_wK);
	assert (pieces[King_Square(cc_b)] == pi_bK);

	/* king in check */ 
	int square= King_Square(!to_move);
	if (Square_Attacked(square, !to_move))
	{
		String square_text= Format("%c%c",
					   File(square) + 'a',
					   Rank(square) + '1');

		Error::Set(Format(to_move == cc_w
				  ? _("White king on %s is in capture")
				  : _("Black king on %s is in capture"),
				  square_text()));
		return false;
	}

	/* in_check */
	in_check= Square_Attacked(King_Square(to_move), to_move); 

	/* Check that there is no pinned pawn that can be captured
	 * e.p.  (That would not be legal because the king would have
	 * been in check while the other side was to move.  LEGAL
	 * assumes that fact to do an optimization.)
	 */
	if (ep_poss)
	{
		assert (pieces[ep_poss] == Piece(1^to_move, pi_P));
		pieces[ep_poss]= pi_empty;
		bool error= false;
		if (Attacked_From(ep_poss))
		{
			Error::Set(_("Last double pawn move is not legal"));
			error= true;
		}
		pieces[ep_poss]= Piece(1^to_move, pi_P);
		if (error)  return false;
	}

	legal= true;
	return true;
}

bool Position::Attacked_From(int square)
{
	int king_square= king_squares[to_move];
	int king_file= File(king_square);
	int king_rank= Rank(king_square);
	int file= File(square);
	int rank= Rank(square);

	int step= 0;
	int piece_mask;
	if (king_file == file)  /* same file */
	{
		step= rank-king_rank >= 0 ? 10 : -10;
		piece_mask= (1 << pi_R) | (1 << pi_Q);
	}
	else if (king_rank == rank) /* same rank */ 
	{
		step= file-king_file >= 0 ?  1 :  -1;
		piece_mask= (1 << pi_R) | (1 << pi_Q);
	}
	else if (king_file - file == king_rank - rank) /* a1-h8 type diagonal */
	{
		step= file-king_file >= 0 ? 11 : -11;
		piece_mask= (1 << pi_B) | (1 << pi_Q);
	}
	else if (file - king_file == king_rank - rank) /* a8-h1 type diagonal */
	{
		step= file-king_file >= 0 ?  -9 :  9;
		piece_mask= (1 << pi_B) | (1 << pi_Q);
	}

	if (step)
	{
		king_square += step;
		while (pieces[king_square] == pi_empty) 
			king_square += step;

		if (Is_Piece_Of_NColor(pieces[king_square], to_move)    &&
		    (((1 << (pieces[king_square] & pi_type)) & piece_mask)))
		{
			return true;
		}
	}
	return false;
}

bool Position::Attacks(int square)
{
	assert (Is_Inside_Square(square));
	assert (Is_Piece_Of_NColor(pieces[square], to_move));
	
	int pt= pieces[square] & pi_type;

	assert (pt >= 0 && pt <= pi_K);

	if (pt == pi_K)  return false;  /* kings don't give check */ 

	/* pawn */ 
	if (pt == pi_P)
	{
		if (to_move)
		{
			if (pieces[square+ 9] == pi_bK)
				return true;
			if (pieces[square+11] == pi_bK)
				return true;
			return false;
		}
		else
		{
			if (pieces[square- 9] == pi_wK)
				return true;
			if (pieces[square-11] == pi_wK)
				return true;
			return false;
		}
	}

	int king= Piece(to_move, pi_K);

	/* knight */ 
	if (pt == pi_N)
	{
		for (int i= 8;  i < 16;  ++i)
		{
			if (pieces[square + moves_offset[i]] == king)
				return true;
		}
		return false;
	}
	
	/* long moving pieces */
	int king_square= king_squares[to_move];
	int file= File(square), rank= Rank(square);
	int king_file= File(king_square), king_rank= Rank(king_square);

	int step= 0, piece_mask;
	if (file == king_file)
	{
		step= king_rank-rank >= 0 ? 10 : -10;
		piece_mask= (1 << pi_R) | (1 << pi_Q);
	}
	else if (rank == king_rank) /* same rank */ 
	{
		step= king_file-file >= 0 ?  1 :  -1;
		piece_mask= (1 << pi_R) | (1 << pi_Q);
	}
	else if (king_file - file == king_rank - rank) /* a1-h8 type diagonal */
	{
		step= king_file-file >= 0 ? 11 : -11;
		piece_mask= (1 << pi_B) | (1 << pi_Q);
	}
	else if (file - king_file == king_rank - rank) /* a8-h1 type diagonal */
	{
		step= file-king_file >= 0 ?  9 :  -9;
		piece_mask= (1 << pi_B) | (1 << pi_Q);
	}

	if (!step)
		return false;

	if (! (piece_mask & (1 << (pieces[square] & pi_type))))
		return false; 

	do
		square += step;
	while (pieces[square] == pi_empty);

	return king_square == square; 
}

bool Position::Would_Be_Legal(bool material_correct) const
{
	Position p(*this);

	if (!material_correct)
	{
		p.Update_Material();
	}

	return p.Set_Legal(); 
}

void Position::Invert()
{
	to_move ^= 1;
	if (ep_poss)  ep_poss= Square_Invert(ep_poss);

	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		for (int rank= sq_1;  rank <= sq_4;  ++rank)
		{
			int square= Make_Square(file, rank);
			int osquare= Make_Square(file, 7-rank);
			int tmp= pieces[square];
			pieces[square]= pieces[osquare];
			pieces[osquare]= tmp;
			if (pieces[square] & pi_color)
				pieces[square] ^= pi_color;
			if (pieces[osquare] & pi_color)
				pieces[osquare] ^= pi_color;
		}
	}

	for (int i= 0;  i < 3;  ++i)
	{
		int tmp= moved[i];
		moved[i]= moved[i+3];
		moved[i+3]= tmp;
	}
	int tmp= king_squares[cc_w];
	king_squares[cc_w]= king_squares[cc_b];
	king_squares[cc_b]= tmp;
	if (to_move)
		++previous_move_count;
	else
		--previous_move_count; 
}

void Position::Mirror()
{
	assert (! legal);
	if (ep_poss)   ep_poss= Square_Mirror(ep_poss);

	for (int rank= sq_1;  rank <= sq_8;  ++rank)
	{
		for (int file= sq_a;  file <= sq_d;  ++file)
		{
			int square= Make_Square(file, rank);
			int osquare= Make_Square(7-file, rank);
			int tmp= pieces[square];
			pieces[square]= pieces[osquare];
			pieces[osquare]= tmp;
		}
	}

	king_squares[cc_w]= Square_Mirror(king_squares[cc_w]);
	king_squares[cc_b]= Square_Mirror(king_squares[cc_b]);
}

void Position::Piece_Array::operator = (const Piece_Array &array)
{
	for (int rank= sq_1;  rank <= sq_8;  ++rank)
		memcpy(pieces+Make_Square(sq_a, rank), 
		       array.pieces+Make_Square(sq_a, rank), 
		       sizeof(int8_t)*8);
}

bool Position::Piece_Array::operator == (const Piece_Array &piece_array) const
{
	for (int rank= sq_1;  rank <= sq_8;  ++rank)
		if (memcmp(pieces+Make_Square(sq_a, rank),
			   piece_array.pieces+Make_Square(sq_a, rank),
			   8))
			return false;
	return true; 
}

