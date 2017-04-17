#ifndef OPENING_BOOK_HH
#define OPENING_BOOK_HH

#include "book.hh"

class Opening_Book
	:  virtual public Pointed
{
public:
	bool Open();
	bool Is() const {return book.Is(); }
	const Book &Get_Book() const {return book; }
	bool Get(const Game &);
	/* Return whether we have moves */

	void Get(const Game &, 
		 Vector <String> &names, 
		 int op_flags,
		 String &eco);
	bool Has_Position() const 
		{
			assert (Is());
			return has; 
		}
	bool Has_Moves() const 
		{
			assert (Is());
			return has && book().current;
		}
	bool Choose_Random_Opening(move_t &small_move, bool omit_bad= false) const;
	String Name(move_t small_move, 
		    int op_flags, 
		    bool ignore_detail= false) const;
	/* Ignore EP, CAPT and PROM from SMALL_MOVE. 
	 */

private:
	Book book;
	bool has;
	/* Whether the position is in the book.  Set in Get(*).
	 * Undefined when the book is closed.  */ 

	static String Decode(String comment, int op_flags); 
	/* Remove the special opening name characters and translate the names.
	 * Remove alternatives when they should not be displayed according to
	 * OP_FLAGS.  '*' and '+' are not present. 
	 */
	static String Decode(String comment);
	/* COMMENT is one comment in the PGN text format.  Translate to current
	 * language. 
	 */
};

#endif /* ! OPENING_BOOK_HH */ 
