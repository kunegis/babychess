#include "format_move_name.hh"

#include "format.hh"

String File_Name(int file, const Notation &notation) 
{
	assert (file >= 0 && file < 8);
	const char *p= notation.texts[i_files]();
	while (file && *p)
	{
		--file;
		/* Assumes UTF-8 */ 
		++p;
		while ((*p & 0xC0) == 0x80)  ++p;
	}
	if (!*p)  return 'a'+file;
	const char *q= p+1;
	while ((*q & 0xC0) == 0x80)  ++q;
	return String(p, q-p);
}

String Format_End_Piece_Type(int piece_type,
			     const Notation &notation,
			     Notation_Settings settings)
{
	assert (piece_type >= 0 && piece_type < 6);
	if (piece_type != pi_P
	    || settings.flags & nos_pawn_sign)
		return notation.texts[i_P + piece_type];
	else
		return "";
}

String Format_Move_Name(const Move_Name &move_name,
			const Notation &notation,
			const Notation_Settings &settings)
{
	String result;

	/* Main move */ 
	if ((move_name.special == 2 || move_name.special == 3)
	    && ! (settings.flags & nos_no_castling))
	{
		result= notation.texts[i_short_castling+move_name.special-2];
	}
	else
	{
		/* Piece sign */ 
		result += Format_End_Piece_Type(move_name.piece_type,
						notation, settings);

		if (settings.type == n_long)  
		{
			String file_name= File_Name(move_name.from_file,
						    notation);
			result += Format("%s%c", 
					 file_name(),
					 '1'+move_name.from_rank);

			if (move_name.capture == 0)
			{
				/* This is an EN dash */ 
				result += "–";
			}
			else
			{
				result += notation.texts[i_capture]; 
			}

			if (move_name.capture != 0
			    && settings.flags & nos_long_capture_name)
				result += Format_End_Piece_Type
					(move_name.capture - 1,
					 notation,
					 settings);
			file_name= File_Name(move_name.to_file, notation);
			result += Format("%s%c", 
					 file_name(),
					 '1'+move_name.to_rank);
		}
		else /* short, descriptive */ 
		{
			/* Disambiguation */ 
			if (move_name.precision % 2)
				result += File_Name(move_name.from_file, notation);
			if (move_name.precision / 2)
				result += '1' + move_name.from_rank;

			/* Capture */
			if (move_name.capture)
			{
				if (move_name.piece_type == pi_P)  
				{
					result += File_Name(move_name.from_file,
							    notation);
				}
				if (! (settings.flags & nos_no_short_capture)
				    && ! (settings.flags & nos_short_capture_end))
					result += notation.texts[i_capture];
			}

			/* To square */ 
			String file_name= File_Name(move_name.to_file, notation);
			result += Format("%s%c", 
					 file_name(),
					 '1'+move_name.to_rank); 

			/* Capture */ 
			if (move_name.capture
			    && ! (settings.flags & nos_no_short_capture)
			    && settings.flags & nos_short_capture_end)
				result += notation.texts[i_capture];
		}

		/* En passant */ 
		if (! (settings.flags & nos_no_ep) && move_name.special == 1)
		{
			result += " ";
			/* a non-breaking space in UTF-8 */ 
			result += notation.texts[i_ep];
		}
		
		/* Promotion */ 
		if (move_name.special >= 4)
		{
			if (notation.texts[i_promotion])
				result += notation.texts[i_promotion];
			result +=
		Format_End_Piece_Type(move_name.special - 3, notation, settings);
		}
	}

	/* Check and similar */ 
	if (move_name.check && ! (settings.flags & nos_no_check))
	{
		if (move_name.force)
			result += '#';
		else
		{
			result += notation.texts[i_check]; 
			if (!(settings.flags & nos_no_double_check) && move_name.check == 2)
				result += notation.texts[i_check];
		}
	}

	return result;
}

