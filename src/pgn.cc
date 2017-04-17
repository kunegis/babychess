#include "pgn.hh"

#include "string.hh"
#include "parse_move.hh"
#include "read_line.hh"
#include "error.hh"
#include "fen.hh"
#include "format.hh"
#include "notation.hh"
#include "nag.hh"
#include "gettext.hh"
#include "stdio.hh"
#include "line_reader.hh"

#include "format_move_name.hh"

FILE *PGN::file= NULL; 

int PGN::char_count;
String PGN::word_buffer; 

bool PGN::Write_Raw_Tags(const Hash_Table &tags)
{
	for (int i= 0 ; i < *tags; ++i)
	{
		fprintf(file, "[%s \"%s\"]\n",
			tags.Key(i)(), tags.Value(i)()); 
	}

	/* End-of-header empty line */
	if (0 > stdio_putc('\n', file))
		return false;

	return true;
}

bool PGN::Write_Tags(const Game &game)
{
	assert (game.info.result >= 0 &&
		game.info.result < 4);

	String date_text= game.info.Format_Date();

	fprintf(file,
		"[Event \"%s\"]\n"
		"[Site \"%s\"]\n"
		"[Date \"%s\"]\n"
		"[Round \"%s\"]\n"
		"[White \"%s\"]\n"
		"[Black \"%s\"]\n"
		"[Result \"%s\"]\n",
		game.info.event.Is() ? game.info.event() : "?",
		game.info.site .Is() ? game.info.site () : "?",
		date_text(),
		game.info.round.Is() ? game.info.round() : "?",
		game.info.players[cc_w].Is() ? game.info.players[cc_w]() : "?",
		game.info.players[cc_b].Is() ? game.info.players[cc_b]() : "?",
		Game_Info::Get_Result_Text(game.info.result)()); 

	/* FEN tag */
	if (game.Starting_Position().Is())
	{
		String fen= FEN(game.Starting_Position()()); 
		fprintf(file,
			"[FEN \"%s\"]\n"
			"[SetUp \"1\"]\n",
			fen()); 
	}

	/* other tags and empty line */
	Write_Raw_Tags(game.info.tags); 

	return true;
}

bool PGN::Write_Word(String word)
{
	assert (word.Is());

	if (word_buffer.Is()         &&
	    word_buffer[-1] != '('   &&
	    word[0]      != ')')
		/* Words cannot stick together */
	{
		if (! Flush())
			return false;
		word_buffer= word;
	}
	else
	{
		word_buffer += word;
	}
	
	return true; 
}

bool PGN::Write_Comment(String comment)
{
	assert (comment);
	if (! Write_Word(Format("{%s}", comment())))
		return false;
	return true; 
}

bool PGN::Write_Variations(const Vector <Dynamic <Variation> > &variations)
{
	for (int i= 0;  i < *variations;  ++i)
	{
		const Variation &variation= variations[i]();

		if (! Write_Word("("))  return false;
		if (! Write_Variation(variation))  return false;
		if (! Write_Word(")"))  return false;
	}
	return true;
}

bool PGN::Write_Variation(const Variation &variation)
{
	String comment= variation.Get_Comment(0);
	if (comment)
		if (! Write_Comment(variation.Get_Comment(0)))
			return false;

	bool previous_obstructions= false;
	/* Whether comments or subvariations were written after the
	 * last move.  If true, the move number must be written even
	 * in the case of a black move. 
	 */

	for (int i= 0;  i < *variation;  ++i)
	{
		/* Move number */

		int overall_move_number= i + variation.Get_Previous_Move_Count();
		if (overall_move_number % 2 == 0)
		{
			if (! Write_Word(Format("%d.",
						overall_move_number / 2 + 1)))
				return false;
		}
		else if (i == 0 || previous_obstructions)
			/* first move is black */
		{
			if (! Write_Word(Format("%d...",
						overall_move_number / 2 + 1)))
				return false;
		}
			

		/* The move */  
		String move_name=
		Format_Move_Name(variation.Get_Move_Name(i),
				 san_notation, san_notation_settings);
		if (! Write_Word(move_name))
			return false;

		/* NAG */

		int nag= variation.Get_NAG(i);
		if (nag != 0)
			if (! Write_Word(Format("$%d", nag)))
				return false;

		/* Comment */ 

		String comment= variation.Get_Comment(i + 1);
		if (comment)
		{
			if (! Write_Comment(comment))
				return false;
			previous_obstructions= true;
		}
		else 
			previous_obstructions= false;

		/* Variations */ 

		if (variation.Have_Variations(i))
		{
			if (! Write_Variations(variation.Get_Variations(i)))
				return false;
			previous_obstructions= true;
		}
	}

	return true; 
}

bool PGN::Write(FILE *new_file,
		const Game &game)
{
	file= new_file; 

	/* Tags */
	if (! Write_Tags(game))
		return false;

	/* Moves */
	char_count= 0;
	/* characters on one line.  Lines in the PGN file must have a limited 
	 * length. 
	 */
	word_buffer= "";
	
	if (! Write_Variation(game.Main_Variation()))
		return false;
	if (! Write_Word(game.info.Get_Result_Text()))
		return false;
	if (! Flush())
		return false;
	if (0 > stdio_putc('\n', file))
		return false;

	/* empty line */
	if (0 > stdio_putc('\n', file))
		return false;

	file= NULL; 

	return true; 
}

bool PGN::Parse_Header(Hash_Table &tags,
		       String &carry)
{
	int tag_count= 0;
	/* Corresponds to *TAGS, but count also tags that could not be
	 * parsed.  
	 */

	bool wrote_tags= false;

	while (true)
		/* Read one line  */
	{
		String line;
		if (carry.Is())
		{
			line= carry;
			carry= "";
		}
		else
		{
			if (! Read_Line(file, line))
			{
				return wrote_tags;
			}				
			if (! line.Is())
			{
				if (tag_count != 0)
				{
					return true;
				}
			}
		}

		if (line.Is())
		{
			Parser parser(line);
			parser.Skip_Space();
			if (parser.End())
			{
				if (tag_count != 0)
					return true;
			}
			else 
			{
				if (parser.Skip_Char() == '[')
				{
					++ tag_count;
					parser.Skip_Space();
					String name= parser.Skip_Word();
					if (name.Is())
					{
						parser.Skip_Space();
						String value;
						if (parser.Skip_String(value))
						{
							tags.Set(name, value);
							wrote_tags= true;
						}
						else
						{
							if (value.Is())
							{
								tags.Set(name, value); 
								wrote_tags= true;
							}
							else
							{
								String word= parser.Skip_Word_Excluding
									(']');
								if (word.Is())
								{
									tags.Set(name, word); 
									wrote_tags= true;
								}
							}
						}
					}
				}
				else
				{
					carry= line;
					return tag_count != 0;
				}
			}
		}
	}
}

void PGN::Parse_Move(String word,
		     Game &game,
		     Parser &parser)
{
	assert (word.Is());

	/* Ignore end-of-game markers */
	if (word == "1-0"      ||
	    word == "0-1"      ||
	    word == "1/2"      ||
	    word == "1/2-1/2"  ||
	    word == "*")
		goto end;

	/* Move consists of:
	 *      - move number:  "1.", "2...", ""
	 *      - move in SAN
	 *      - textual NAG:  "", "!", ...
	 */
	{
		unsigned i= 0;
		while (isdigit(word[i]))  ++i;
		while (word[i] == '.')    ++i;

		if (i == *word)
			goto end;
		unsigned j= i;
		/* Check for characters that are used in textual NAGs */ 
		while (word[j] && NULL == strchr("!?[]~@^", word[j]))
//		       word[j] != '!' &&
//		       word[j] != '?' &&
//		       word[j] != '
			++j;
		
		int sm= ::Parse_Move(word.Sub(i, j - i),
				     game.Current_Generator());
		if (sm >= 0)
		{
			game.Append_Move(sm);
		}
		if (j < *word)
		{
			int nag= Parse_NAG(word.Sub(j, *word - j));
			game.Set_NAG(nag);
		}
	}

 end:
	parser.Skip(*word);
	parser.Skip_Space();
}

bool PGN::Parse_Moves(const Vector <String> &moves,
		      Game &game)
{
	if (*moves == 0)
	{
		Error::Set(_("Game is empty"));
		return false; 
	}

	bool in_comment= false;

	for (int i= 0;  i < *moves;  ++i)
	{
		String line= moves[i];

		if (! (line.Is() || in_comment))
			/* empty line */
			return true;
		if (line[0] == '%')  continue;
		Parser parser(line);
		do
		{
			if (in_comment)
			{
				game.Current_Variation().Append_Comment
					(game.Current_Variation().Length(), "\n");
				String comment= parser.Skip_Excluding('}');
				game.Current_Variation().Append_Comment
					(game.Current_Variation().Length(), 
					 comment);
				if (parser.Next_Char() == '}')
				{
					in_comment= false;
					parser.Skip_Char();
				}
			}

			if (in_comment)  continue;

			parser.Skip_Space();
			switch (parser.Next_Char())
			{
			case '{':
			{
				parser.Skip_Char();
				String comment= parser.Skip_Excluding('}');
				game.Current_Variation().Append_Comment
					(game.Current_Variation().Length(),
					 comment);
				if (parser.Next_Char() == '}')
				{
					parser.Skip_Char();
				}
				else
				{
					in_comment= true; 
				}
				break;
			}
			case '(':
				/* take back last move */
				if (game.Current_Location() == 0)
					return false;

				game.Go_Backward_In_Current(); 

				if (! game.Append_Variation())
					return false;
				parser.Skip_Char();
				parser.Skip_Space();
				break;
			case ')':
			{
				game.Leave_Variation();
				bool f= game.Go_Forward();
				assert (f); 

				parser.Skip_Char();
				parser.Skip_Space();
				break;
			}
			case '$':
			{
				parser.Skip_Char();
				int nag;
				if (1 == parser.Skip_Format("%d", &nag))
					if (game.Current_Location() != 0)
						game.Set_NAG(nag);
				parser.Skip_Space();
				break;
			}
			default:  /* move name */ 
				String word= parser.Next_Word_Excluding("{()"); 
				if (word.Is())
				{
					Parse_Move(word, game, parser);
				}
				break;
			}
		} 
		while (! parser.End());
	}
	game.Normalize();
	return true;
}

bool PGN::Parse(FILE *new_file, 
		Game &game,
		String &carry)
{
	assert (file == NULL);

	file= new_file; 

	Raw_PGN raw;
	bool have_header= Parse_Raw(raw, carry); 
	file= NULL;

	String fen= raw.Tag("FEN");

	if (fen.Is())
	{
		if (! game.New(fen))
			return false;
	}
	else
		game.New();

	if (! Parse_Moves(raw.Moves(), game))
	{
		if (! have_header)
			return false; 
	}

	game.info.Set(raw.Tags());

	return true; 
}

bool PGN::Parse_Raw(Raw_PGN &raw,
		    String  &carry)
{
	Hash_Table tags;

	bool have_header= Parse_Header(tags, carry); 
	raw.Set_Tags(tags);
	raw.Normalize();
	raw.Clear_Moves();
	int brace_level= 0;
	
	Line_Reader line_reader(file);

	do
	{
//		String line;
		Parser parser;
		if (carry.Is())
		{
			Object_Reset(Parser, & parser, carry);
//			line= carry;
			carry= "";
		}
		else
		{
			if (line_reader.Read())
			{
				Object_Reset(Parser, &parser,
					     line_reader.Line(), 
					     line_reader.Length());
			}
			else
//			if (! Read_Line(file, line))
			{
				return have_header || raw.Moves().Is(); 
			}
		}
//		unsigned index= 0;
		parser.Skip_Space();
//		while (isspace(line[index]))
//			++ index;
		if (/*index == *line*/parser.End() && brace_level == 0) /* line is empty */ 
		{
			return have_header || raw.Moves().Is(); 
		}
		if (parser[0] == '[' && brace_level == 0)
		{
			carry= parser.Text();
			return true;
		}
		while (/*index < *line*/ ! parser.End())
		{
			if (/*line[index]*/ parser[0] == '{')
				++ brace_level;
			if (/*line[index]*/ parser[0] == '}')
			{
				if (brace_level > 0)
					-- brace_level;
			}
//			++ index;
			parser.Skip_Char();
		}
		raw.Append_Move_Line(parser.Text());
	}
	while (true);
}

bool PGN::Read_Raw(const Raw_PGN &raw,
		   Game &game)
{
	String fen= raw.Tag("FEN");
	if (fen.Is())
	{
		if (! game.New(fen))
			game.New(); 
	}
	else
		game.New();

	Parse_Moves(raw.Moves(), game); 
	/* If Parse_Moves() returns FALSE, no moves were in the
	 * game.  That's OK because a PGN game may consist of only
	 * headers. 
	 */

	game.info.Set(raw.Tags());

	return true; 
}

bool PGN::Write_Raw(const Raw_PGN &raw_pgn)
{
	if (! Write_Raw_Tags(raw_pgn.Tags()))
		return false;

	for (int i= 0;  i < raw_pgn.Line_Count();  ++i)
	{
		if (! stdio_put_string(raw_pgn.Line(i), file))
			return false;
		if (0 > stdio_putc ('\n', file))
			return false;
	}

	return  0 <= stdio_putc('\n', file);
}

bool PGN::Parse_Raw(FILE *new_file,
		    Raw_PGN &raw,
		    String &carry)
{
	assert (file == NULL);

	file= new_file; 
	bool result= Parse_Raw(raw, carry);
	file= NULL;
	return result; 
}

bool PGN::Write_Raw(FILE *new_file,
		    const Raw_PGN &raw_pgn)
{
	assert (file == NULL); 
	file= new_file; 
	bool result= Write_Raw(raw_pgn);
	file= NULL; 
	return result; 
}

bool PGN::Flush()
{
	if (! word_buffer.Is())  
		return true;
	
	/* Remaining word is too long */ 
	if (char_count && (char_count + *word_buffer + 1 > line_length))
	{
		if (0 > stdio_putc('\n', file))  
			return false;
		char_count= 0;
	}
	else
	{
		if (char_count != 0)
		{
			if (0 > stdio_putc(' ', file))
				return false;
			char_count += 1;
		}
	}
	char_count += *word_buffer;
	
	if (! stdio_put_string(word_buffer, file))
		return false;

	return true;
}

void PGN::Open(FILE *new_file)
{
	assert (file == NULL); 

	file= new_file;
}

void PGN::Close()
{
	assert (file != NULL);

	assert (word_buffer == ""); 

	file= NULL; 
}
