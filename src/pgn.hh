#ifndef PGN_HH
#define PGN_HH

/* Portable Game Notation.  Text notation for chess games. 
 *
 * See <http://www.clark.net/pub/pribut/standard.txt>. 
 */

/* PGN is "open" when "stream" is non-NULL. 
 */

/* Deficiencies of the PGN format:
 *
 *  - There is no easy way to tell where the next game begins.  One has to
 *    search for an empty line, but empty lines can be contained in
 *    comments.
 *  - PGN mandates the Latin-1 encoding.  It should either mandate UTF-8, or
 *    allow the encoding to be specified.  We 
 */

/* International characters:  PGN mandates that Latin-1 be used for PGN
 * comments.  In this implementation, no conversion is being done.
 * Characters are saved in whatever format the GTK edit box returns,
 * and that is UTF-8. 
 *
 * It would be interesting to know what other programs do.  Is Latin-1
 * really enough for all users?  
 */

#include "game.hh"
#include "raw_pgn.hh"
#include "parser.hh"

#include <stdio.h>

class PGN
{
public:

	static bool Parse(FILE *file, 
			  Game &game,
			  String &carry); 
	/* Read one PGN game from FILE and put it in GAME.  Because the
	 * only way to recognize the end of the game is to read one line
	 * too much, the last line read which is not part of the game
	 * may be put in CARRY. 
	 */

	static bool Write(FILE *,
			  const Game &game);
	static bool Parse_Raw(FILE *,
			      Raw_PGN &raw,
			      String &carry);
	/* CARRY as in Parse(). */
	static bool Write_Raw(FILE *,
			      const Raw_PGN &raw_pgn); 
	static bool Read_Raw(const Raw_PGN &raw,
			     Game &game); 
	static void Open(FILE *);
	static void Close();

private:

	enum Last_Word 
	{lw_nothing, lw_close, lw_open, lw_move, lw_number};

	enum {line_length= 80}; 
	/* As defined by the PGN standard.  */

	/* Set and cleared in the public
	 * functions and used in the other functions.   */
	static FILE *file;

	/* Set in Write().   */
	static int char_count;
	static String word_buffer;
	/* The last word that was added, not already written to file. 
	 */
	
	static bool Write_Tags(const Game &game);
	/* Including the empty line after the tags.  */

	static bool Write_Word(String word);
	/* Write a space or a newline, respecting PGN line length, and then
	 * write WORD.  */

	static bool Write_Comment(String comment);

	static bool Write_Variations(const Vector <Dynamic <Variation> > &variations);

	static bool Parse_Header(Hash_Table &tags,
				 String &carry);
	/* Return whether at least one tag was read.  */
	static void Parse_Move(String move_text,
			       Game &game,
			       Parser &parser);
	static bool Parse_Moves(const Vector <String> &moves,
				Game &game); 
	static bool Parse_Raw(Raw_PGN &raw,
			      String &carry);
	static bool Write_Raw(const Raw_PGN &raw_pgn); 
	static bool Write_Raw_Tags(const Hash_Table &tags);
	static bool Write_Variation(const Variation &variation);

	static bool Flush();
	/* Write REM_WORD */
};

#endif /* ! PGN_HH */
