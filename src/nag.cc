#include "nag.hh"

/* The correct unicode symbols will be used when Linux has more complete
 * fonts. 
 */

#include "format.hh"
#include "gettext.hh"

#include <stdlib.h>
#include <string.h>

struct NAG_Entry
{
	const char *symbol, *ascii, *text; 
};

NAG_Entry nags[nag_count]=

{/*      Symbol  Ascii    Meaning                              */ 

	/* 0 */
 	{NULL,   NULL,    NULL                                },

	/* 1 */ 
	{"!",    NULL,    N_("good move")                     },
	{"?",    NULL,    N_("poor move")                     },

	/* 3 */ 
	/* There are four Unicode characters representing the next
	 * four, but the normal representation looks better. */ 
	{"!!",   NULL,    N_("very good move")                },  
	{"??",   NULL,    N_("a blunder")                     },  
	{"!?",   NULL,    N_("speculative move")              },  
	{"?!",   NULL,    N_("questionable move")             },  

	/* 7 */ 
	{"□",    "[]",    N_("forced move")                   }, 
	/* synonym:  only move */ 
	{NULL,   NULL,    N_("singular move")                 }, 
	{NULL,   NULL,    N_("worst move")                    },

	/* 10 */ 
	{"∼",    "~",     N_("drawish position")              }, 
	{"=",    NULL,    N_("equal chances, quiet position") },
	{"⇄",    "->/<-", N_("equal chances, active position")}, 
	{"∞",    "oo",    N_("unclear position")              },  

	/* 14 */ 
	{"+=",   NULL,    N_("White has a slight advantage")  }, 
	/* on top of each other 2A72 ⩲ */
	{"=+",   NULL,    N_("Black has a slight advantage")  }, 
	/* on top of each other 2A71 ⩱ */

	/* 16 */ 
	{"±",    "+/-",   N_("White has a moderate advantage")},
	{"∓",    "-/+",   N_("Black has a moderate advantage")}, 

	/* 18 */ 
	/* in the following we use the correct Unicode minus sign */
 	{"+−",   "+-",    N_("White has a decisive advantage")},
	{"−+",   "-+",    N_("Black has a decisive advantage")},
	{"+−−",  "+--",   N_("White has a crushing advantage")},
	{"−−+",  "--+",   N_("Black has a crushing advantage")},

	/* 22 */ 
	{"⊙",    "@",     N_("White is in Zugzwang")          }, 
	{"⊙",    "@",     N_("Black is in Zugzwang")          }, 

	{NULL,   NULL,    N_("White has a slight space advantage")}, 
	{NULL,   NULL,    N_("Black has a slight space advantage")},

	/* 26 */ 
	{"○",    "o",     N_("White has a moderate space advantage")}, 
	{"○",    "o",     N_("Black has a moderate space advantage")},

	{NULL,   NULL,    N_("White has a decisive space advantage")},
	{NULL,   NULL,    N_("Black has a decisive space advantage")},
	{NULL,   NULL,    N_("White has a slight time (development) advantage")},
	{NULL,   NULL,    N_("Black has a slight time (development) advantage")},

	/* 32 */ 
	{"↻",    NULL,    N_("White has a moderate time (development) advantage")}, 
	{"↻",    NULL,    N_("Black has a moderate time (development) advantage")},

	{NULL,   NULL,    N_("White has a decisive time (development) advantage")},
	{NULL,   NULL,    N_("Black has a decisive time (development) advantage")},

	/* 36 */ 
	{"↑",    "^",     N_("White has the initiative")}, 
	{"↑",    "^",     N_("Black has the initiative")},

	{NULL,   NULL,    N_("White has a lasting initiative")},
	{NULL,   NULL,    N_("Black has a lasting initiative")},

	/* 40 */ 
	{"→",    "->",    N_("White has the attack")},  
	{"→",    "->",    N_("Black has the attack")},  

	{NULL,   NULL,    N_("White has insufficient compensation for material deficit")},
	{NULL,   NULL,    N_("Black has insufficient compensation for material deficit")},

	/* 44 */ 
	/* These two don't seem to be included in Unicode. */ 
	{"∞=",  "&/=",    N_("White has sufficient compensation for material deficit")},
	/* infinity above equal sign */ 
	{"=∞",  "=/&",    N_("Black has sufficient compensation for material deficit")},
	/* equal sign above infinity */ 
 };

int Parse_NAG(String text)
{
	for (int i= 1;  i < nag_count;  ++i)
	{
		if (nags[i].symbol && 0 == strcmp(text(), nags[i].symbol))   
			return i;
		if (nags[i].ascii  && 0 == strcmp(text(), nags[i].ascii))
			return i;
	}
	return 0;
}

String NAG(int i)
{
	assert (i >= 0);
	if (i == 0)   return "";
	if (i >= nag_count || ! nags[i].symbol)  return Format("$%d", i);
	return nags[i].symbol;
}

const char *NAG_Description(int nag)
{
	assert (nag >= 0 && nag < nag_count);
	return _(nags[nag].text); 
}
