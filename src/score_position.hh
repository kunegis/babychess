#ifndef SCORE_POSITION_HH
#define SCORE_POSITION_HH

/* A chess position that keeps tracks of the score and other
 * information.  All additional info is stored in the Score_Info
 * struct.  Playing a move changes that info.  Taking moves back must
 * be done with Take_Back().  This function must passed the position
 * info which must be saved before the move is played. 
 */

#include "position.hh"

class Score_Info
{
public:

	int score;
	/* From the viewpoint of the side to move. 
	 */

	int pawns[8][2];
	/* Number of pawns on each line. 
	 * index:   file, color
	 */

	int game_type;
};

class Score_Position
/* A position that keeps track of:
 *         - an evaluation of the position (always from the side to move)
 */
	:  public Position
{
public:

	Score_Position()
		:  Position(0, 0)
		{ }
	/* raw */
	Score_Position(const Position &position);

	const Score_Info &Get_Info() const
		{
			return info; 
		}
	int Get_Score() const
		{
			return info.score;
		}

	void operator << (const Move &move);

	void Take_Back(const Move &move, Score_Info old_info)
		{
			Position::operator >> (move);
			info= old_info;
		}

	void Reset(const Position &);

private:

	enum {double_pawn_penalty= 10};

	Score_Info info; 

	void Add_Pawn(int file, int color);
	void Remove_Pawn(int file, int color);
	void Initialize();
};

#endif /* ! SCORE_POSITION_HH */ 
