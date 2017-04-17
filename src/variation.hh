#ifndef VARIATION_HH
#define VARIATION_HH

#include "string.hh"
#include "dynamic.hh"
#include "vector.hh"
#include "generator.hh"

class Variation
/* A game of chess, whithout storing the starting position or game
 * info. 
 */
	:  public Pointed 
{
public:

	Variation(int new_previous_move_count= 0)
		:  previous_move_count(new_previous_move_count)
		{
		}
	Variation(Variation &variation,
		  int location);
	/* Take the moves from LOCATION to the end, and cut
	 * VARIATION. 
	 */

	int Length() const
		/* Number of moves in the variation */
		{ return *moves;  }
	int operator * () const
		{ return Length();}
	
	String Get_Comment(int index) const;

	void Set_Comment(int i, String comment);
	void Append_Comment(int location, String comment);

	bool Append_Variation(int location);
	/* 0 <= LOCATION < LENGTH.
	 */
	const Vector <Dynamic <Variation> > &Get_Variations(int location) const
		/* Must first have checked Have_Variations(LOCATION). 
		 */
		{
			return variations[location];
		}
	Vector <Dynamic <Variation> > &Get_Variations(int location) 
		{
			return variations[location];
		}
	bool Have_Variations(int location) const
		{
			return *variations > location &&
				*variations[location]; 
		}

	void Free();
	bool Append_Move(const Generator &,
			 int move_index);
	/* At the end of this variation. 
	 */
	Move &operator [] (int i)
		{
			return moves[i]; 
		}
	const Move &operator [] (int i) const
		{
			return moves[i]; 
		}

	void Cut(int new_length);
	Move_Name Get_Move_Name(int i) const
		{
			return names[i];
		}
	int Get_Previous_Move_Count() const
		{
			return previous_move_count;
		}
	void Remove_Variation(int location,
			      int variation_index);

	int Get_NAG(int i) const
		{
			if (i >= *nags)  return 0;
			return nags[i]; 
		}

	void Set_NAG(int location, int nag);
	bool Append_Moves(Variation &variation);
	/* Append the moves of VARIATION to THIS and emtpy VARIATION. 
	 */
	bool Append_Variation(int location,
			      Dynamic <Variation> &variation);
	/* VARIATION is taken.   */
	void Normalize();
#ifndef NDEBUG
	void Check_Variations() const;
#else
	void Check_Variations() const {}
#endif

private:

	Vector <Move> moves;
	/* Moves of game.  Size of MOVES is size of variation. 
	 */

	Vector <Move_Name> names;
	/* Names of the game moves. 
	 * *NAMES == *MOVES
	 */

	Vector <int8_t> nags;
	/* Numeric annotation glyph.  Parallel to MOVES.  
	 * *NAGS <= *MOVES
	 * A NAG of 0 corresponds to no NAG.  Unallocated NAGs
	 * represent 0. 
	 */

	Vector <String> comments;
	/* Comments.
	 *
	 * *COMMENTS <= *MOVES + 1 
	 *
	 * Comments can contain the character '}'. (Which is impossible
	 * to put in a PGN file.)
	 *
	 * COMMENTS[0] comes before the first move.
	 * For i > 0, COMMENTS[i] comes after move number i. 
	 */

	Vector <Vector <Dynamic <Variation> > > variations;
	/* Subvariations. 
	 *
	 * *VARIATIONS <= *MOVES 
	 *
	 * All dynamics are.  
	 *
	 * VARIATIONS[i] replaces MOVES[i], is written after MOVES[i]
	 * in PGN, and must be played before MOVES[i].
	 */

	int previous_move_count;
	/* Number of previous moves.
	 */

	void Normalize_Subvariation(int location,
				    int variation_index);
};

#endif /* ! VARIATION_HH */
