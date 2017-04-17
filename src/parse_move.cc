#include "parse_move.hh"

#include "notation.hh"

int Parse_Move(String           text, 
	       const Generator &generator)
{
	/* castling */
	if (text[0] == 'O' || /* big Oh */
	    text[0] == 'o' || /* little Oh */
	    text[0] == '0')   /* zero */
	{
		int type= (*text >= 3 && text[3] == '\0') ? 0 : 1;
		for (int i= 0;  i < *generator;  ++i)
		{
			int castling= generator.Get_Move(i).castling;
			if (castling && (castling % 2 == type))
				return i;
		}
		return -1;
	}

	/* Squares.  A value of 8 indicates that no information was
	 * found.  */
	int from_file= 8;
	int from_rank= 8;
	int to_file= 8;
	int to_rank= 8;

	/* Piece type;  8 when not found  */
	int piece_type= 8;
	int capture= 8;
	int promotion= 8;

	/* Whether there was an equal sign  */
	bool found_equal= false;

	/* Found a sign for capture */
	bool found_capture= false;
	
	/* Read all characters in forward direction.  Find characters
	 * that correspond to piece type and equal sign.  */
	for (unsigned i= 0;  i < *text;  ++i)
	{
		char c= text[i];
		if (c >= 'A' && c <= 'Z')
		{
			int pt= san_chars[c - 'A'];

			if (pt != 8)
			{
				if (i > 0 && text[i - 1] == '=')
					promotion= pt;
				else if (piece_type == 8)
					piece_type= pt;
				else if (promotion == 8)
					promotion= pt;
			}
		}
		else if (c == 'q' ||
			 c == 'r' ||
			 c == 'n')
			/* Cannot match `b' because `b' is also the
			 * name of a file (on the chess board). 
			 */
		{
			if (promotion == 8)
				promotion= san_chars[c - 'a'];
		}
		else if (c == '=')
			found_equal= true;
		else if (c == 'x' || c == ':')
		{
			found_capture= true; 
		}
	}

	/* Read all characters in backward direction.  Find square names.  
	 */
	for (int i= *text - 1;  i >= 0;  --i)
	{
		char c= text[i];
		if (c >= 'a' && c <= 'h' /* && c != 'x' */ )
		{
			if (to_file == 8)
				to_file= c - 'a';
			else if (from_file == 8)
				from_file= c - 'a';
		}
		if (c >= '1' && c <= '8')
		{
			if (to_rank == 8)
				to_rank= c - '1';
			else if (from_rank == 8)
				from_rank= c - '1';
		}
	}

	/* Search all legal moves, assign each a value based on the
	 * information found, and return the move with the highest
	 * value.   */

	/* Index of current move with best value  */
	int found= -1;

	/* Current best value */
	int found_value= -1;

	for (int i= 0;  i < *generator;  ++i)
	{
		Move move= generator[i]; 

		int value= 0;

		/* Check each type of information and change VALUE
		 * accordingly. 
		 */
		if (from_file != 8)
			value += from_file == File(move.from) ? 2 : -2;
		if (from_rank != 8)
			value += from_rank == Rank(move.from) ? 2 : -2;
		if (to_file != 8)
			value += to_file == File(move.to) ? 2 : -2;
		if (to_rank != 8)
			value += to_rank == Rank(move.to) ? 2 : -2;
		
		int piece= generator.Get_Position()[move.from];
		assert (piece & pi_color);
		int move_pt= piece & pi_type; 

		if (piece_type == 8)
		{
			if (move_pt == pi_P)
				++ value;
		}
		else
			value += (move_pt == piece_type) ? 2 : -2;

		if (found_capture)
		{
			if (move.captured & pi_color)
				++ value; 
		}
		else
		{
			if (capture == 8)
			{
			}
			else
			{
				if (move.captured & pi_color)
				{
					if (capture == (move.captured & pi_type))
						++ value;
					else
						-- value; 
				}
				else
					-- value;
			}
		}

		if (promotion != 8)
			value += ((move.promotion & pi_istype) == promotion) ? 1 : -1;
		if (found_equal &&
		    (move.promotion & pi_color))
			++ value;

		if (value > found_value)
		{
			found= i;
			found_value= value;
		}
	}

	return found;
}
