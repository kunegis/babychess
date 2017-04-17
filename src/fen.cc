#include "fen.hh"

#include "verbose.hh"
#include "format.hh"
#include "error.hh"
#include "parser.hh"
#include "gettext.hh"
#include "notation.hh"

#include <ctype.h>

String FEN(const Position &position)
{
	String result;

	/* First field:  piece placement */

	for (int rank= sq_8; rank >= sq_1; -- rank)
	{
		if (rank != sq_8)  result += '/';

		int empty_count= 0;

		for (int file= sq_a; file <= sq_h; ++ file)
		{
			int piece= position[Make_Square(file, rank)];
	    
			if (piece & pi_color)
			{
				if (empty_count != 0)
				{
					result += '0' + empty_count;
					empty_count= 0;
				}

				const char *const chars= "PNBRQK";
				if (piece & pi_white)
					result += chars[piece & pi_type];
				else
					result += 'a' - 'A' + chars[piece & pi_type];
			}
			else
			{
				++ empty_count;
			}
		}

		if (empty_count != 0)
			result += '0' + empty_count;
	}

	result += ' ';

	/* Second field:  active color */

	if (position.To_Move() == cc_w)
		result += 'w';
	else
		result += 'b';

	result += ' ';

	/* Third field:  castling availability */

	unsigned osize= *result;
	if (position.Moved(1) == 0 &&
	    position[sq_e1] == pi_wK)
	{
		if (position.Moved(2) == 0 &&
		    position[sq_h1] == pi_wR)
			result += 'K'; 
		if (position.Moved(0) == 0 &&
		    position[sq_a1] == pi_wR)
			result += 'Q'; 
	}
	if (position.Moved(4) == 0 &&
	    position[sq_e8] == pi_bK)
	{
		if (position.Moved(5) == 0 &&
		    position[sq_h8] == pi_bR)
			result += 'k'; 
		if (position.Moved(3) == 0 &&
		    position[sq_a8] == pi_bR)
			result += 'q'; 
	}

	if (*result == osize)
		result += '-';

	result += ' ';

	/* Fourth field:  en passant target square */

	if (position.EP_Poss() == 0)
		result += '-';
	else
	{
		result += 'a' + File(position.EP_Poss()) - sq_a;
		if (Rank(position.EP_Poss()) == sq_4)
			result += '3';
		else
			result += '6';
	}

	result += ' ';

	/* Fifth field:  halfmove clock */

	result += Format("%d", position.Halfmove_Clock()); 

	result += ' ';

	/* Sixth field:  fullmove number */

	result += Format("%d", position.previous_move_count / 2 + 1);

	return result;
}

String Printify_Char(char c)
{
	if (isprint(c))
		return c;
  
	if (c == '\0')
		return "^@";

	if (c >= 1 && c <= 26)
		return (String('^')
			+ (char)('A' + c - 1));

	if (c == 27)
		return "^[";

	if (c == 28)
		return "^\\";

	if (c == 29)
		return "^]";

	if (c == 30)
		return "^^";

	if (c == 31)
		return "^_";

	return '?';
}

bool Parse_FEN(String    text,
	       Position &position)
{
	if (!text)
	{
		Error::Set("Empty String");
		return false;
	}

	Parser parser(text);
	if (position.Legal())  position.Set_Not_Legal();

	/* First field */
	parser.Skip_Space();
	String first_field= parser.Skip_Word();
	
	int file= 0, rank= 7;
	for (unsigned i= 0;  i < *first_field;  ++i)
	{
		char c= first_field[i];

		switch (c)
		{
		case '1' ... '8':
		{
			int n= c - '0';
			if (file + n > 8)
			{
				Error::Set(_("More than eight squares on one row"));
				return false;
			}
			for (int j= 0;  j < n;  ++j)
				position[Make_Square(file + j, rank)]= pi_empty;
			file += n;
			break;
		}

		case 'a' ... 'z':
         	case 'A' ... 'Z':
		{
			if (file > 7)
			{
				Error::Set(_("More than eight squares on one row"));
				return false;
			}
			int color= c >= 'A' && c <= 'Z' ? cc_w : cc_b;
			int piece_type= San_Char(c);
			if (piece_type == 8)
			{
				String printified= Printify_Char(c);
				Error::Set(Format(_("Invalid character \"%s\" in the first field"),
						  printified()));
				return false;
			}

			int piece= Piece(color, piece_type);
			int square= Make_Square(file, rank);
			position[square]= piece;
			if (piece_type == pi_K)
			{
				position.Set_King_Square(color, square);
			}
			++ file;
			break;
		}

		case '/':
		{
			if (file < 7)
			{
				Error::Set(_("Less than eight squares on one row"));
				return false;
			}
			if (rank == 0)
			{
				Error::Set(_("More than eight rows"));
				return false;
			}
			file= 0;
			-- rank;
			break;
		}
		
	        default:
		{
			String printified= Printify_Char(c);
			Error::Set(Format(_("Invalid character \"%s\" in the first field"),
					  printified()));
			return false;
		}
	        }
	}

	assert(file <= 8);
	if (rank > 0)
	{
		Error::Set(_("Less than eight rows"));
		return false;
	}

	if (file < 8)
	{
		Error::Set(_("Less than eight squares on one row"));
		return false;
	}

	/* Second field */
	parser.Skip_Space();
	String second_field= parser.Skip_Word();
	position.Set_To_Move(second_field[0] == 'w' ? cc_w : cc_b); 

	/* Third field  (castlings) */
	parser.Skip_Space();
	String third_field= parser.Skip_Word();
	for (int i= 0; i < 6; ++i)
		position.Set_Moved(i, 1);
	for (unsigned i= 0;  i < *third_field;  ++i)
	{
		char c= third_field[i];
		if (c == 'K'                   && 
		    position[sq_e1] == pi_wK   &&
		    position[sq_h1] == pi_wR)
		{
			position.Set_Moved(1, 0);
			position.Set_Moved(2, 0);
		}
		else if (c == 'Q'                 &&
			 position[sq_a1] == pi_wR &&
			 position[sq_e1] == pi_wK)
		{
			position.Set_Moved(1, 0);
			position.Set_Moved(0, 0);
		}
		else if (c == 'k'                 &&
			 position[sq_e8] == pi_bK &&
			 position[sq_h8] == pi_bR)
		{
			position.Set_Moved(4, 0);
			position.Set_Moved(5, 0);
		}
		else if (c == 'q'                  &&
			 position[sq_a8] == pi_bR  &&
			 position[sq_e8] == pi_bK)
		{
			position.Set_Moved(4, 0);
			position.Set_Moved(3, 0);
		}
	}

	/* Fourth field:  ep capture square (eg "e6") */
	parser.Skip_Space();
	String fourth_field= parser.Skip_Word();
	char c= fourth_field[0];
	position.EP_Poss()= 
		((c >= 'a' && c <= 'h') 
		 ? Make_Square(c - 'a', position.To_Move() == cc_w ? 4 : 3) 
		 : 0);

	/* Fifth field:  halfmove clock     */
	parser.Skip_Space();
	String hm_text= parser.Skip_Word();
	int new_hm;
	if (1 != sscanf(hm_text(), "%d", & new_hm))
		new_hm= 0;
	else
		if (new_hm < 0)
			new_hm= 0;
	position.Set_Halfmove_Clock(new_hm); 

	/* Sixth field:  fullmove number  */
	parser.Skip_Space();
	String sixth_field= parser.Skip_Word();
	unsigned fullmove;
	if (1 != sscanf(sixth_field(),
			"%u",
			& fullmove))
		fullmove= 1;
	if (fullmove < 1)
		fullmove= 1;

	position.previous_move_count= (fullmove - 1) * 2 + position.To_Move();

	position.Update_Material();
	return position.Set_Legal(); 
}
