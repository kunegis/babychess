#ifndef GAME_HH
#define GAME_HH

/* See etc/GAME for more documentation. */

#include "dynamic.hh"
#include "generator.hh"
#include "pointer.hh"
#include "hash_table.hh"
#include "game_info.hh"
#include "variation.hh"
#include "notation.hh"
//#include "notation_settings.hh"

class Game
	:  public Pointed
{
public:
	Game_Info info; 

	Game() {}
	Game(const Game &);
	~Game(); 

	bool Is() const
		{ return *location_stack; }

	void New();
	/* New game in starting position.  May be open.  */
	bool New(String fen);
	/* New game starting from FEN. */ 
	bool New(const Position &);
	/* New game from POSITION.  The position must be legal.   */
	void Close();

	bool Append_Move(int index);
	/* To the end of current variation.  Location must be the end
	 * of the current variation. 
	 */
	bool Go_To_Start();
	/* Return TRUE when position changed.
	 */
	bool Go_Forward();
	/* Return TRUE when position changed.
	 */
	bool Go_To_End();
	/* Return TRUE when position changed.
	 */
	void Remove_End();

	void Set_Comment(String comment)
		{ variation_stack[-1]().Set_Comment(location_stack[-1], comment);}
	void Append_Comment(String comment)
		{ Set_Comment(Comment() + comment); }

	String Comment() const
		{ return variation_stack[-1]().Get_Comment(location_stack[-1]);  }
	
	const Position &Current_Position() const
		{
			assert (Is()); 
			return position;
		}

	const Generator &Current_Generator() const 
		{
			assert (Is());
			return generator; 
		}

	const Dynamic <Position> &Starting_Position() const
		/* Starting position.  Not defined if game begins from
		 * standard starting position. 
		 */
		{
			return starting_position; 
		}
	
	int First_Move() const
		/* Which side moves first in the game.  */
		{
			assert (Is());
			return (starting_position.Is() 
				? starting_position().To_Move()
				: cc_w);
		}

	const Variation &Main_Variation() const
		{
			return variation; 
		}
	Variation &Current_Variation()
		{
			return variation_stack[-1](); 
		}
	const Variation &Current_Variation() const
		{
			return variation_stack[-1](); 
		}
	int Current_Location() const
		{
			return location_stack[-1]; 
		}
	Move_Name Current_Move_Name(int current_location) const
		{
			return variation_stack[-1]().Get_Move_Name
				(current_location); 
		}
	void Append_Variation(int move_index);

	void operator = (const Game &); 
	int Overall_Location() const;

	const Move &Overall_Move(int i) const;

	Move_Name Overall_Move_Name(int i) const;

	int Overall_Length() const
		{
			return variation_stack[-1]().Get_Previous_Move_Count()
				+ variation_stack[-1]().Length();
		}
	int Variation_Depth() const
		{
			return *location_stack; 
		}
	bool Is_Overall_Variation_Move(int overall_location) const;

	bool Go_To_Overall_Location(int location);
	/* Return whether a variation was left.
	 */
	int Current_Length() const
		{
			return variation_stack[-1]().Length(); 
		}
	const Move &Current_Move(int loc) const
		{
			return variation_stack[-1]()[loc];
		}
	void Remove_Variation();
	/* Remove current variation.  */

	void Set_NAG(int nag);
	/* To last move in current variation.  Current location must
	 * not be 0.  */
	bool Leave_Variation();
	/* Return whether a variation was left.  */
	bool Go_Backward();
	bool Make_Main();

	bool Make_Parent();
	void Remove_Variation(int index);
	void Normalize(); 

	bool Append_Variation(); 
	/* Append a variation that replaces the next move in the current
	 * variation and enter the newly created variation.  Leaves
	 * the game non-normalized. 
	 */
	void Go_Backward_In_Current();
	/* Take back last move of current variation.
	 * Must have LOCATION_STACK[-1] > 0.
	 * Return TRUE when position changed.
	 * May make the position non-normalized.  Don't update the
	 * generator. 
	 */
	void Enter_Variation(int variation_index);
	/* Non-normalized.  Doesn't change the position. 
	 */
	void Play_Variation(int i);
	String Overall_Move_Text(int i, 
				 const Notation &notation, 
				 const Notation_Settings &) 
		const;
	/* The name of the move, followed by the glyph and "*" if the
	 * moves begins a subvariation. 
	 */
	String Format_Variation_Move(int variation_index, const Notation
				     &, const Notation_Settings &) const;
	const Move &Variation_Move(int i) const;
	void Get_Variation_Location(int overall_location,
				    int &variation, 
				    int &variation_location) 
		const;
	/* Given an OVERALL_LOCATION, return the index of the
	 * corresponding variation and the location within that
	 * variation. 
	 */
	int Overall_NAG(int overall_location) const
		{
			int v, i;
			Get_Variation_Location(overall_location, v, i);
			return variation_stack[v]().Get_NAG(i);
		}
	String Overall_Comment(int overall_location) const
		{
			int v, i;
			Get_Variation_Location(overall_location, v, i);
			return variation_stack[v]().Get_Comment(i);
		}
	String Format_Last_Move(const Notation &, 
				const Notation_Settings &) const;
	const Vector <int> &Location_Stack() const
		{
			return location_stack;
		}
	const Vector <Pointer <Variation> > &Variation_Stack() const
		{
			return variation_stack; 
		}
	void Set_Starting_Previous_Move_Count(int new_starting_previous_move_count);
	const char *To_Move_Description() const;
	
	void Append_New_Move(int move)
		{
			if (Current_Length() != Current_Location())
				Append_Variation(); 
			Append_Move(move);
		}

private:

	Variation variation;

	Vector <int> location_stack;
	/* Position in the game.  
	 * LOCATION_STACK[i] is the position in VARIATION_STACK[i].
	 * For all i, 0 <= LOCATION_STACK[i] <= *VARIATION_STACK[i]. 
	 */

	Vector <Pointer <Variation> > variation_stack;
	/* *VARIATION_STACK == *LOCATION_STACK.
	 * For all i, VARIATION_STACK[i] != NULL. 
	 * VARIATION_STACK[0] == & VARIATION.  
	 * For i > 0, VARIATION_STACK[i] is a subvariation at position 
	 * LOCATION_STACK[i - 1] of VARIATION_STACK[i - 1]. 
	 */

	Position position;
	Generator generator;

	Dynamic <Position> starting_position;
	/* If not allocated, the game begins from the starting
	 * position.     
	 */
	
	void Reset_Position();
	/* Set POSITION to the game's starting position 
	 */
	void Go_Up();
	/* Leave the current variation.  
	 */

	static void Commentify(String &comment);
};

#endif /* ! GAME_HH */
