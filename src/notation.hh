#ifndef NOTATION_HH
#define NOTATION_HH


#include "pointer.hh"
#include "string.hh"

#include <stdio.h>

/* The string index */
enum 
{
	/*                             Description        SAN equiv.*/

	i_ep,                       /* en passant                   */
	i_castling,   
	i_short_castling=i_castling,/* short castling     O-O       */
	i_long_castling,            /* long  castling     O-O-O     */

	i_P,                        /* pieces             PNBRQK    */
	i_last_piece= i_P+6-1,
	/* Pieces are not stored as one string because in some
	 * notations, two characters are used for one piece (e.g. "Kt"
	 * in old English)
	 */

	i_capture,                  /* capture sign       x         */
	i_promotion,                /* promotion sign     =         */
	i_color,                    /* white piece                  */
	i_color_b,                  /* black piece                  */
	i_check,                    /* check sign         +         */
	i_files,                    /* all file names     abcdefgh  */

	i_count
};


/* Chess notation in a specific language.  Contains strings used for
 * displaying moves in that language.  
 */
class Notation
	:  public Pointed
{
public:
	String texts[i_count];

	Notation() {}
	Notation(const char *const *);
	/* NULL terminated
	 */

	void Reset();
	static const char *Default_Conf_Name();
};

extern const int san_chars[26];
/* For each letter from A to Z, the piece type that corresponds to the
 * letter.  If the letter 'x' is a character for a piece in SAN, then
 * san_chars['x'-'a'] contains the piece type.  If the characters is
 * not used in SAN, then the character might correspond to a piece
 * type in another language.  Letters that do not match any languages
 * are indicated by 8. 
 */

Inline int San_Char(char c)
	/* Interface to SAN_CHARS. 
	 */
{
	if (c >= 'A' && c <= 'Z')
		c += ('a' - 'A');

	if (! (c >= 'a' && c <= 'z'))
		return 8;

	return san_chars[c - 'a'];
}

bool Read_Stream(FILE *, Notation *);
bool Write_Stream(FILE *, const Notation *);


/* Notation contains the parts of the notation that are not language
 * specific. 
 */
enum 
{
	/* Type */ 
	/* Short == 0 is the default for both SAN and user visible
	 * notation. 
	 */
	n_short, n_long, 
	n_count,

	/* Flags */ 
	/* For user visible notation, the flags are 0.  For SAN, they
	 * are set in SAN. 
	 */
	
	nos_pawn_sign           =1<<  0, 
	/* Show pawn sign */
	nos_no_ep               =1<<  1,
	/* Show "e.p."    */
	nos_no_check            =1<<  2,
	/* Don't show check and mate */ 
	nos_no_short_capture    =1<<  3, 
	/* Don't show capture sign in short notation */
	nos_short_capture_end   =1<<  4, 
	/* In short notation, put capture sign at the end.  Sometimes
	 * used in German notation in conjunction with the colon as the
	 * capture sign, e.g. ed5: instead of e×d5.  */
	nos_long_capture_name   =1<<  5, 
	/* Show type of captured piece in long notation */ 
	nos_no_double_check     =1<<  6,
	/* Indicate double check by doubling the check sign */ 
	nos_no_castling         =1<<  7,
	/* Don't treat castling as special move, i.e. use e1-g1... */ 
	
	nos_count               =     8,

//	nos_user= nos_ep | nos_double_check
};


struct Notation_Settings
	:  public Pointed
{
	int type;  /* n_* */
	int flags; /* ns_* */

	Notation_Settings() {}
	Notation_Settings(int new_type, 
			  int new_flags)
		:  type(new_type),
		   flags(new_flags)
		{}

	static const char *Default_Conf_Name()
		{  return NULL; }
	void Reset() 
		{type= 0;  flags= 0; }
};

bool Read_Stream(FILE *, Notation_Settings *);
bool Write_Stream(FILE *, const Notation_Settings *);

extern Notation san_notation;
extern Notation_Settings san_notation_settings; 

#endif /* ! NOTATION_HH */
