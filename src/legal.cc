#include "legal.hh"

#include "moves.hh"
#include "verbose.hh"

bool Legal::just_check;
bool Legal::strict_legal;
Pointer <Position> Legal::sposition;
Pointer <Vector <Move> > Legal::smoves;
int Legal::move_index;
int Legal::square;
int Legal::to_move;
int Legal::flags;

bool Legal::Append(int to, 
		   int captured,
		   int promotion, 
		   int ep, 
		   int castling)
{
	Move &move= smoves()[move_index];
	move.from= square;
	move.to= to;
	move.captured= captured;
	move.ep_poss= sposition->EP_Poss();
	move.promotion= promotion;
	move.ep= ep;
	move.castling= castling;
	move.piece= sposition()[square];
	move.halfmove_clock= sposition->Halfmove_Clock();
	move.check= sposition->In_Check();

	bool append= true;
	/* Wether the move must be appended.  Set to FALSE
	 * when it is not legal and only legal moves are
	 * generated.  
	 */
	
	if (strict_legal)
	{
		bool in_check= sposition->In_Check();
		sposition->Light_Play(move);
		int cc= sposition->To_Move();

		if (in_check || ((move.piece & pi_type) == pi_K))
		{
			/* KING_SQUARE is not updated in the position.  */
			int king_square= sposition->King_Square(cc);
			if ((move.piece & pi_type)  == pi_K)
				king_square= to;
			append= ! sposition->Square_Attacked(king_square, cc);
		}
		else /* King was not in check and hasn't moved.  Check
		      *	if the move would put the king in check */
		{
			bool a= sposition->Attacked_From(move.from);
			append= ! a;
		}

		sposition->Light_Take_Back(move);
	}
	
	if (append)
	{
		++ move_index; 
		if (just_check)
			return true;
		else
			return false;
	}
	else
		return false;
}

void Legal::Generate(Position &position,
		     Vector <Move> &moves,
		     bool new_strict_legal, 
		     bool new_just_check)
{
	just_check= new_just_check;
	strict_legal= new_strict_legal;
	smoves= moves;
	sposition= position;

	smoves->Set(just_check ? 1 : max_leg);
	move_index= 0;

	to_move= position.To_Move();
	flags= 0;
  
	static int cans[2]= {pi_can_white, pi_can_black};
	int can= cans[to_move];

	for (int rank= sq_1;  rank <= sq_8;  ++ rank)
	{
		for (int file= sq_a;  file <= sq_h;  ++ file)
		{
			square= Make_Square(file, rank);
			int from_piece= position[square];
			if (Is_Piece_Of_Color(from_piece, to_move))
			{
				switch (from_piece & pi_type)
				{
				case pi_P: /* pawn */
					if (from_piece & pi_white)
						/* white pawn */
					{
						if (Rank(square) == sq_7) /* white pawn on 7th rank */
						{
							int osize= move_index; 
							/* advance */ 
							if (position[square + 10] == pi_empty)
							{
								for (int pt= pi_Q;  pt >= pi_N;  --pt)
									if (Append(square + 10, pi_empty, Piece(cc_w, pt)))
										goto end;
							}
							/* capture on top left */ 
							if (position[square + 9] & pi_black)
							{
								for (int pt= pi_Q;  pt >= pi_N;  -- pt)
									if (Append(square + 9, position[square + 9], Piece(cc_w, pt)))
										goto end;
							}
							/* capture on top right */ 
							if (position[square + 11] & pi_black)
							{
								for (int pt= pi_Q;  pt >= pi_N;  --pt)
									if (Append(square + 11, position[square + 11], Piece(cc_w, pt)))
										goto end;
							}
							if (osize != move_index)  flags |= gn_promotion;
						}
						else /* white pawn on ranks 2-6 */ 
						{
							/* advance on square */ 
							if (position[square + 10] == pi_empty)
								if (Append(square + 10, pi_empty))
									goto end;
							/* double advance */ 
							if (Rank(square) == sq_2   &&
							    position[square + 10] == pi_empty &&
							    position[square + 20] == pi_empty)
								if (Append(square + 20, pi_empty))
									goto end;
							/* capture on top left */ 
							if (position[square + 9] & pi_black)
								if (Append(square + 9, position[square + 9]))
									goto end;
							/* capture on top right */
							if (position[square + 11] & pi_black)
								if (Append(square + 11, position[square+11]))
									goto end;
							/* e.p. capture */ 
							if (Rank(square) == sq_5)
							{
								/* to the left */ 
								if (position.EP_Poss() == square - 1)
									if (Append(square + 9, Piece(cc_b, pi_P), pi_empty, square - 1))
										goto end;
								/* to the right */ 
								if (position.EP_Poss() == square + 1)
									if (Append(square + 11, Piece(cc_b, pi_P), pi_empty, square + 1))
										goto end;
							}
						}
					}
					else /* black pawn */
					{
						if (Rank(square) == sq_2)
							/* black pawn on second rank */
						{
							int osize= move_index; 
							if (position[square - 10] == pi_empty)
							{
								for (int pt= pi_Q;  pt >= pi_N;  --pt)
									if (Append(square - 10, pi_empty, Piece(cc_b, pt)))
										goto end;
							}
							/* capture on bottom right */ 
							if (position[square - 9] & pi_white)
							{
								for (int pt= pi_Q;  pt >= pi_N;  --pt)
									if (Append(square - 9, position[square - 9], Piece(cc_b, pt)))
										goto end;
							}
							/* capture on bottom left */ 
							if (position[square - 11] & pi_white)
							{
								for (int pt= pi_Q;  pt >= pi_N;  --pt)
									if (Append(square - 11, position[square - 11], Piece(cc_b, pt)))
										goto end;
							}
							if (osize != move_index)  flags |= gn_promotion;
						}
						else /* black pawn on rank 7-3 */ 
						{
							/* advance one square */ 
							if (position[square - 10] == pi_empty)
								if (Append(square - 10, pi_empty))
									goto end;
							/* double advance */ 
							if (Rank(square) == sq_7
							    && (position[square - 10] == pi_empty)
							    && (position[square - 20] == pi_empty))
								if (Append(square - 20, pi_empty))
									goto end;
							/* capture on bottom right */ 
							if (position[square - 11] & pi_white)
								if (Append(square-11, position[square - 11]))
									goto end;
							/* capture on bottom right */ 
							if (position[square - 9] & pi_white)
								if (Append(square - 9, position[square - 9]))
									goto end;
							/* e.p. capture */ 
							if (Rank(square) == sq_4)
							{
								/* to the left */ 
								if (position.EP_Poss() == square - 1)
									if (Append(square - 11, Piece(cc_w, pi_P), pi_empty, square - 1))
										goto end;
								/* to the right */ 
								if (position.EP_Poss() == square + 1)
									if (Append(square - 9, Piece(cc_w, pi_P), pi_empty, square + 1))
										goto end;
							}
						}
					}
					break;
				case pi_N: /* knight */
					for (int i= 8; i < 16;  ++i)
					{
						int to_square= square + moves_offset[i]; 
						int to_piece= position[to_square];
						if (to_piece & can)
							if (Append(to_square, to_piece))
								goto end;
					}
					break;

				case pi_B:  /* long move */
				case pi_R:     
				case pi_Q:
				{
					int pt= position[square] & pi_type; 
					int j0= moves_range[0][pt - 2];
					int j1= moves_range[1][pt - 2]; 
					assert (j0 < j1);
					for (int i= j0;  i < j1;  ++i) 
					{
						int s= square + moves_offset[i];
						while (position[s] == pi_empty)
						{
							if (Append(s, pi_empty, pi_empty, 0))
								goto end;
							s += moves_offset[i];
						}
						int to_piece= position[s];
						if (to_piece & can)
							if (Append(s, to_piece, pi_empty , 0))
								goto end;
					}
					break;
				}

				case pi_K: /* king */
		  
					/* normal king moves */ 
					for (int i= 0; i < 8; ++i)
					{
						int s= square + moves_offset[i];
						int to_piece= position[s];
						if (to_piece & can)
							if (Append(s, to_piece))
								goto end;
					}
					/* white castling */
					if (square == sq_e1 && !position.Moved(1))
					{
						if (position[sq_f1] == pi_empty
						    && position[sq_g1] == pi_empty
						    && !position.Square_Attacked(sq_f1, cc_w)
						    && !position.Moved(2)
						    && !position.Square_Attacked(sq_e1, cc_w))
							if (Append(sq_g1, pi_empty, pi_empty, 0, wO_O))
								goto end;
						if (position[sq_d1] == pi_empty
						    && position[sq_c1] == pi_empty
						    && position[sq_b1] == pi_empty
						    && !position.Square_Attacked(sq_d1, cc_w)
						    && !position.Moved(0)
						    && !position.Square_Attacked(sq_e1, cc_w))
							if (Append(sq_c1, pi_empty, pi_empty, 0, wO_O_O))
								goto end;
					}
					/* black castling */
					else if (square == sq_e8 && !position.Moved(4))
					{
						if (position[sq_f8] == pi_empty &&
						    position[sq_g8] == pi_empty &&
						    ! position.Square_Attacked(sq_f8, cc_b) &&
						    ! position.Moved(5)
						    && !position.Square_Attacked(sq_e8, cc_b))
							if (Append(sq_g8, pi_empty, pi_empty, 0, bO_O))
								goto end;

						if (position[sq_d8] == pi_empty
						    && position[sq_c8] == pi_empty
						    && position[sq_b8] == pi_empty
						    && !position.Square_Attacked(sq_d8, cc_b)
						    && !position.Moved(3)
						    && !position.Square_Attacked(sq_e8, cc_b))
							if (Append(sq_c8, pi_empty, pi_empty, 0, bO_O_O))
								goto end;
					}
					break;

				default:
					assert(0);
				}
			}
		}
	}

 end:
	moves= move_index; 
	sposition= NULL;
	smoves= NULL;
}
