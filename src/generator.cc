#include "generator.hh"

bool Generator::New(const Position &new_position,
		    bool no_names)
{
	position= new_position;

	/* Generate legal moves */ 

	Position tmp_position= new_position;
	moves.Free();
	Legal::Generate(tmp_position, moves, true, false);

	flags= Legal::flags;

	names.Free(); 

	if (no_names)
		return true; 

	/* Generate move names */

	if (! names.Set(*moves)) return false;

	for (int i= 0;  i < *moves;  ++i)
	{
		names[i].color= new_position.To_Move(); 
		names[i].piece_type= tmp_position[moves[i].from] & pi_type;
		int piece= tmp_position[moves[i].to];
		names[i].capture= (piece & pi_color)  ? 1 + (piece & pi_type): 0;
		names[i].from_file= File(moves[i].from);
		names[i].from_rank= Rank(moves[i].from);
		names[i].to_file= File(moves[i].to);
		names[i].to_rank= Rank(moves[i].to);

		if (moves[i].promotion & pi_color)
			names[i].special= 3 + (moves[i].promotion & pi_type);
		else if (moves[i].ep)
		{
			names[i].special= 1;
			names[i].capture= 1;
		}
		else if (moves[i].castling)
			names[i].special= moves[i].castling % 2 + 2;
		else
			names[i].special= 0;
      
		if ((tmp_position[moves[i].from] & pi_type) != pi_P)
		{
			bool d= false, dl= false, dr= false;
			for (int j= 0;  j < *moves;  ++j)
			{
				if (j != i                     &&
				    moves[i].to == moves[j].to   &&
				    (tmp_position[moves[i].from] == tmp_position[moves[j].from]))
				{
					d= true;
					if (File(moves[i].from)==File(moves[j].from))
						dl= true;
					if (Rank(moves[i].from)==Rank(moves[j].from))
						dr= true;
				}
			}
			if (d)
			{
				if (dl)
				{
					if (dr)
						names[i].precision= 3;
					else
						names[i].precision= 2;
				}
				else
					names[i].precision= 1;
			}
			else
				names[i].precision= 0;
		}

		tmp_position << moves[i];
		int check= tmp_position.In_Check(true);
		assert (check == 0 || check == 1 || check == 2);
		names[i].check= check;

		Vector <Move> tmp_moves;
		Legal::Generate(tmp_position, tmp_moves, true, true);
		names[i].force= *tmp_moves ? 0 : 1;

		tmp_position >> moves[i];
	}

	return true; 
}

int Generator::Square_Move(int from,  int to)  const
{
	assert (Is_Inside_Square(from));
	assert (Is_Inside_Square(to));

	for (int i= 0;  i < **this;  ++i)
	{
		/* ep moves can have the captured piece as the
		 * destination square. */
		if (from == Get_Move(i).from
		    && (to == Get_Move(i).to
			|| to == Get_Move(i).ep))
			return i; 
	}
	return -1;
}

int Generator::Simple_Notation_Move(String move_text) 
	const
{
	if (move_text[0] < 'a' || move_text[0] > 'h'  ||
	    move_text[1] < '1' || move_text[1] > '8'  ||
	    move_text[2] < 'a' || move_text[2] > 'h'  ||
	    move_text[3] < '1' || move_text[3] > '8')
		return -1;

	int
		from=  Make_Square(move_text[0] - 'a', move_text[1] - '1'),
		to=    Make_Square(move_text[2] - 'a', move_text[3] - '1');
  
	int index= Square_Move(from, to);

	switch (move_text[4])
	{
	case 'n':  case 'N':  index += 3;  break;
	case 'b':  case 'B':  index += 2;  break;
	case 'r':  case 'R':  index += 1;  break;
	}

	if (index >= **this)
		return -1;

	return index; 
}


void Generator::Free() 
{
	assert (Is());
	position= NULL;
	moves= 0;
	names= 0; 
}

int Generator::Small_Move(move_t small_move) const
{
	int from= small_move & 0xff;
	int to  = (small_move >> 8) & 0xff;
	int promotion_piece= (small_move >> 24) & 0xf;
	int offset= (promotion_piece & pi_empty) ? 0 : (pi_Q-promotion_piece);
	return offset + Square_Move(from, to);
}

