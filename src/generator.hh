#ifndef GENERATOR_HH
#define GENERATOR_HH

/* A list of legal moves and a list of move names.  */

#include "position.hh"
#include "legal.hh"
#include "move_name.hh"
#include "vector.hh"
#include "simple_notation.hh"

class Generator
	:  public Pointed
{
public:

	Generator()  { }
	~Generator()   {  }

	bool Is() const 
		{  return position.Is();   }
	bool New(const Position &position,
		 bool no_names= false);
	/* POSITION must be valid until the next call to New() or
	 * until destruction.  On error, FALSE is returned and the
	 * generator remains unchanged.  */

	void Free();
	/* Must be.  */

	/* New() must have been called at least once before the
	 * following functions can be called.  */
	const Move &Get_Move(int i) const 
		{
			assert (Is() && i >= 0 && i < *moves);
			return moves[i];
		}
	const Move &operator [] (int n) const {  return Get_Move(n);  }
	int operator * () const
		{
			assert(Is());
			return *moves;
		}

	Move_Name Get_Move_Name(int i) const
		{
			assert (Is());
			assert (i >= 0 && i < *moves);
			return names[i];
		}

	String Get_Simple_Move_Name(int index) const 
		{ return Simple_Notation(Get_Move(index)); }

	int Square_Move(int from,  int to)  const;
	/* Search the corresponding move.
	 * Return the index of the move found.  Return -1 if there is
	 * no such move.  If the move is a promotion, return the
	 * first of the four moves in the list (the queen promotion).  */

	int Small_Move(move_t small_move) const;
	
	int Simple_Notation_Move(String move_text) const;
	/* Scan move notation.  Return move index or -1.  */
	const Position &Get_Position() const
		{ return position();  }
	int Flags() const
		{
			assert (Is()); 
			return flags;
		}

private:

	Pointer <const Position> position;
	/* may be NULL */

	Vector <Move> moves;
	Vector <Move_Name> names;
	/* Empty or the same size as MOVES. 
	 */
	int flags;
};

#endif /* ! GENERATOR_HH */
