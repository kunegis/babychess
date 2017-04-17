#ifndef POSITION_HH
#define POSITION_HH

/* A chess position. 
 *
 * A position can be in two distinct modes:
 *    - legal mode:    The position is legal and only legal moves can be made.   
 *    - non-legal mode:Any changes can be made to the position. 
 *
 * IN_CHECK, MATERIAL and KING_SQUARE are only consistent in legal
 * mode.  
 *
 * Use Legal() to get the mode, and Set_Legal()/Set_Not_Legal() to
 * change the mode. 
 */

#include "square.hh"
#include "piece.hh"
#include "move.hh"
#include "pointer.hh"
#include "material.hh"

class Position
	:  public Pointed
{
public:

	int previous_move_count; 
	/* Number of halfmoves since beginning of game.  The number is even if
	 * white is to move, else odd. 
	 */

	Position() /* empty position*/
		{
			Init_Outside(); 
			
			legal= false;

			to_move= 0;
			moved[1]= 0;
			moved[4]= 0;

			Empty();
		}
	Position(int /* dummy */ ) 
		/* starting position */
		{
			Init_Outside();  
			legal= true;
			Reset(); 
		}
	Position(int, int) /* raw */
		{  Init_Outside();   }
	int8_t &operator [] (int square) 
		{ return pieces.pieces[square];  }
	int operator [] (int square) const
		{ return pieces.pieces[square];  }
	bool operator == (const Position &p) const
		{
			return previous_move_count == p.previous_move_count 
				&& to_move == p.to_move
				&& ep_poss == p.ep_poss
				&& pieces == p.pieces
				&& (!memcmp(moved, p.moved, sizeof(moved)))
				&& (!memcmp(king_squares, p.king_squares, sizeof(king_squares)))
				&& halfmove_clock == p.halfmove_clock
				&& material == p.material
				&& legal == p.legal
				&& in_check == p.in_check;
		}
	bool operator != (const Position &p) const
		{return ! operator == (p); }

	const int8_t *Pieces() const 
		{ return pieces.pieces;  }
	void Set_Moved(int index, int new_moved)
		{
			assert (!legal);
			assert (0 <= index && index < 6);
			moved[index]= new_moved;
		}
	int Moved(int index) const
		{
			assert(0 <= index && index < 6);
			return moved[index];
		} 
	void Set_King_Square(int color, int square)
		/* Must not be in legal mode.  KING_SQUARE will be
		 * verified on entering legal mode.  */
		{
			assert (!legal);
			assert (color == cc_w || color == cc_b);
			king_squares[color]= square;
		}
	int King_Square(int chess_color) const
		{
			assert (chess_color == cc_w || chess_color == cc_b);
			return king_squares[chess_color];
		}

	void Reset(); 
	/* Reset to the starting position.  (and legal mode)
	 */

	void Empty(); 
	/* Don't change MOVED.  Enter non-legal mode. 
	 */

	void operator << (const Move &); 
	/* Play a move      */
	void operator >> (const Move &); 
	/* Take back a move */

	void Light_Play(const Move &);
	void Light_Take_Back(const Move &);
	/* These two functions play/take back a move, but ONLY change
	 * the pieces.  They are used by LEGAL.  
	 */

	int Square_Attacked(int square, 
			    int color,
			    bool multiple= false) const;
	/* Whether COLOR is attacked on SQUARE.  The return value is
	 * the number of times the square is attacked.  If MULTIPLE is
	 * FALSE, at most one attack is reported. 
	 *
	 * multiple=            FALSE                 TRUE
	 * Return value:  0:  not attacked      not attacked
	 *                1:  attacked          attacked once
	 *                2:  (never returned)  attacked at least twice
	 */

	int In_Check(bool multiple= false) const
		/* Whether the king of the side to move is in check.
		 * The return value is the same as that from
		 * Square_Attacked(). 
		 */
		{
			assert (legal);

			if (!multiple)
				return in_check ? 1 : 0;
			else
				return Square_Attacked(King_Square(to_move), to_move, multiple);
		}

	bool Legal() const
		{ return legal;  }
	void Update_Material();
	/* Must be in non-legal mode:  MATERIAL is adjusted to
	 * correspond to the pieces. 
	 */
	bool Set_Legal();
	/* Enter legal mode.  Return TRUE when
	 * position is legal, or set the error. 
	 *
	 * MATERIAL must be correct.  (Call Update_Material() just
	 * before calling Set_Legal(), or build up MATERIAL while
	 * adding pieces.)   
	 *
	 * KING_SQUARES doesn't need to be correct. 
	 */

	void Remove_Piece(int square)
		/* While in non-legal mode.  Don't update MATERIAL. 
		 */
		{
			assert (!legal);
			assert (Is_Inside_Square(square));
			pieces.pieces[square]= pi_empty;
		}
	int To_Move() const
		{ return to_move;  }
	void Transfer_Piece(int from, int to);
	void Change_To_Move(); 
	int EP_Poss() const
		{ return ep_poss;  }
	int &EP_Poss() 
		{ return ep_poss;  }

	void Insert_Piece(int square, int piece); 
	/* While not in legal mode, but maintain MATERIAL. 
	 */

	int Halfmove_Clock() const
		{ return halfmove_clock;  }
	int Current_Material(int cc, int pt) const
		{ return material(cc, pt);  }
	Material &Current_Material()
		{
			assert (! legal); 
			return material; 
		}

	void Set_To_Move(int new_to_move)
		{
			if (new_to_move == to_move)
				return;
			Change_To_Move(); 
		}
	const Material &Current_Material_Const() const
		{
			return material; 
		}
	void Set_Halfmove_Clock(int new_halfmove_clock)
		{
			halfmove_clock= new_halfmove_clock; 
		}
	void Set_Not_Legal()
		{
			assert (legal);
			legal= false; 
		}
	bool Would_Be_Legal(bool material_correct= false) const;
	/* In non-legal mode.  Check whether the position would be
	 * legal.  If MATERIAL_CORRECT, MATERIAL is assumed to be
	 * correct.
	 */
	bool Attacked_From(int square);
	/* Whether the king to move is attacked from a piece on the
	 * file/rank/diagonal where SQUARE is on.  Only checks B/R/Q.  
	 * Note:  This function is used by LEGAL between Light_Play()
	 * and Light_Take_Back() and thus operates on the king that is
	 * NOT to move.  (but TO_MOVE is currently wrong) 
	 */
	void Invert();
	/* Invert black and white:  c4 becomes c5 etc...
	 * Works in any mode, as the legality isn't affected.
	 * MATERIAL is not affected.  
	 */
	void Mirror();
	/* right / left 
	 * MOVED is not changed. 
	 */
	
	static  bool Is_Move_Square(int square)
		/* Whether S is one of the squares a1, e1, h1, a8, e8 or h8. 
		 */ 
		{
			assert(square >= 0 && square < 120);
			int file= File(square), rank= Rank(square);
			return ((rank == sq_1 || rank == sq_8)
				&& (file == sq_a || file == sq_e || file == sq_h));
		}

	static int Square_Move_Index(int square)
		/* S must be one of a1, e1, h1, a8, e8 or h8.  Return s the index of
		 * this square. 
		 */
		{
			assert(square == sq_a1 || square == sq_e1 || square == sq_h1
			       || square == sq_a8 || square == sq_e8 || square == sq_h8);
			if (square > 32)
				square -= 81;
			else
				square -= 20;
			return square / 3;
		}

private:

	class Piece_Array
	{
	public:
		int8_t pieces[120];
		/* The pieces.  The index is the square number, as defined in
		 * SQUARE.  Squares outside the board are initialized by the
		 * constructor and are never changed, but can be read.
		 */

		void operator = (const Piece_Array &);
		/* Optimized to not copy the squares outside the
		 * board, which never change. 
		 */
		int8_t &operator [] (int i)       {return pieces[i]; }
		int     operator [] (int i) const {return pieces[i]; }
		bool operator == (const Piece_Array &piece_array) const;
		bool operator != (const Piece_Array &piece_array) const
			{  return ! operator == (piece_array); }
	};

	int to_move;
	/* who is to move */

	int ep_poss;
	/* Possible en passant square (e.g. e5), or 0.  */

	Piece_Array pieces;

	int moved[6]; 
	/* By index:  a1, e1, h1, a8, e8, h8 
	 *
	 * For each of these squares, the number of times a piece has
	 * moved on it.  If 0, the square contains the rook or king
	 * from the starting position.   
	 *	   
	 * When editing a position, moved[e1] and moved[e8] should
	 * always be 0, so there is:
	 *     wO-O   <=> moved[0] == 0
	 *     wO-O-O <=> moved[2] == 0
	 *     bO-O   <=> moved[3] == 0
	 *     bO-O-O <=> moved[5] == 0
	 */

	int king_squares[2];
	/* position of kings */

	int halfmove_clock; 

	Material material;
	/* Piece count.  In sync with PIECES.   */
	bool legal;
	/* Whether position is legal.  Set to FALSE in Empty(). 
	 */
	bool in_check;
	/* Whether the king to move is in check.  Only in legal mode. 
	 */

	void Init_Outside();
	/* Set the squares outside the chess board to the appropriate
	 * value.  Called once in every constructor. 
	 */ 
	void Set_Material(Material &);
	bool Attacks(int square);
	/* Wether the king to move is in check by the piece on
	 * SQUARE.  There is always a piece of the color to move on
	 * square. 
	 */
};

#endif /* ! POSITION_HH */
