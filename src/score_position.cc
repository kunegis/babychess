#include "score_position.hh"

#include "position_score.hh"
#include "move_score.hh"
#include "game_type.hh"

Score_Position::Score_Position(const Position &position)
	:  Position(position)
{
	Initialize();
}

void Score_Position::operator << (const Move &move)
{
	info.score += Move_Score(move, (*this)[move.from], info.game_type);

	/* captures */ 
	if (move.captured & pi_color)
	{
		/* capture of pawn */ 
		if ((move.captured & pi_type) == pi_P)
			Remove_Pawn(File(move.to), 1^To_Move());

		/* capture by pawn */ 
		if (((*this)[move.from] & pi_istype) == pi_P)
		{
			Remove_Pawn(File(move.from), To_Move());
			Add_Pawn(File(move.to), To_Move());
		}
	}

	info.score= - info.score;

	info.game_type= Game_Type(Current_Material_Const()); 

	Position::operator << (move);
}

void Score_Position::Add_Pawn(int file, int color)
{
	assert (file >= 0 && file < 8);
	assert (color >= 0 && color < 2);

	if (info.pawns[file][color])
		info.score -= double_pawn_penalty;

	++info.pawns[file][color];
}

void Score_Position::Remove_Pawn(int file, int color)
{
	assert (file >= 0 && file < 8);
	assert (color >= 0 && color < 2);
	assert (info.pawns[file][color] > 0);

	--info.pawns[file][color];
	
	if (info.pawns[file][color])
		info.score += double_pawn_penalty;
}

void Score_Position::Reset(const Position &position)
{
	*this= position;
	Initialize(); 
}

void Score_Position::Initialize()
{
	/* game type */
	info.game_type= Game_Type(Current_Material_Const()); 

	/* score */ 
	info.score= Position_Score((Position &)*this, info.game_type);

	/* pawns */
	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		info.pawns[file][0]= info.pawns[file][1]= 0;
		for (int rank= sq_1;  rank <= sq_8;  ++rank)
		{
			int piece= Pieces()[Make_Square(file, rank)];
			if ((piece & pi_istype) == pi_P)
			{
				if (info.pawns[file][Piece_Color(piece)])
					info.score -= double_pawn_penalty; 
				++info.pawns[file][Piece_Color(piece)];
			}
		}
	}

	/* negate for black */ 
	if (To_Move() == cc_b)
		info.score= - info.score;
}
