#include "opening_book.hh"

#include "install_dir.hh"
#include "filename.hh"
#include "opening_settings.hh"
#include "gettext.hh"
#include "format.hh"
#include "randomize.hh"

#include <ctype.h>

#include <algorithm>

bool Opening_Book::Open()
{
	assert (! Is());
	String book_filename= Install_Dir() 
		/ "share/opening_book/openings.book"; 
	if (! book.Open_Read(book_filename()))  return false;
	has= false; 
	return true;
}

bool Opening_Book::Get(const Game &game)
{
	assert (Is());

	book.Go_To_Start();

	if (game.Starting_Position().Is())
		goto has_not;

	for (int i= 0; i < game.Overall_Location(); ++i)
	{
		if (! book.Go_Forward(game.Overall_Move(i)))
			goto has_not;
	}
	has= true;
	return true; 

 has_not: 
	has= false;
	return false;
}

void Opening_Book::Get(const Game &game,
		       Vector <String> &names,
		       int op_flags,
		       String &eco)
{
//	Verbose_Function();
	assert (Is());

	book.Go_To_Start();
	for (int i= 0;  i <= game.Overall_Length();  ++i)
	{
		String comment= book.Comment(); 

//		Verbose("comment = \"%s\"", comment());
		    
		if (comment)
		{
			int i= 0;
			while (isspace(comment[i]))  ++i;
			if (comment[i] == '[')
			{
				int j= i+1;
				do ++i; while (comment[i] != '\0' && comment[i] != ']');
				Object_Reset(String, &eco, comment()+j, i-j);
				if (comment[i] == ']')  ++i;
			}
			while (isspace(comment[i]))  ++i;
			comment= comment()+i;

//			Verbose("\tremain: \"%s\"", comment());

			if (comment)
			{
				if (*names && names[-1][0] == '*')
					names= -1;

				int ast= 0;
				if (comment[0] == '*')
				{
					ast= true;
					String n= comment()+1;
					comment= n;
				}
				int plus_sign_count= 0;
				while (comment[plus_sign_count] == '+')
					++ plus_sign_count;
				comment= Decode(comment()+plus_sign_count, op_flags);
				if (ast)
				{
					String n= String('*')+comment; 
					comment= n;
				}

//				Verbose("Removing %d plus signs", plus_sign_count); 
				names= std::max((*names - plus_sign_count), 0);
			
				names.Append(comment); 
			}
		}
		if (i < game.Overall_Length())
		{
			Move move= game.Overall_Move(i);
			if (! book.Go_Forward(move))  
				break; 
		}
	}
	if (! (op_flags & op_subvariation))
	{
		if (*names)  names= 1;
	}
	if (*names && names[-1][0] == '*')
	{
		names[-1]= names[-1]()+1; 
	}
	Get(game);
}

bool Opening_Book::Choose_Random_Opening(move_t &small_move,
					 bool omit_bad) 
	const
{
//	Verbose_Function();
//	Verbose_Int(omit_bad);

	assert (Is());

	/* Count current moves */ 
	Book::usigf_t next= book().current;
	int count= 0 /* , sum= 0 */ ;
	while (next)
	{
		++count;
		next= book[next].move.cnext;
	}
	if (count == 0)  return false;

	/* Choose randomly */ 
	int ends[count], index= 0;
	next= book().current;
	while (next)
	{
		Book::sigf_t score= book[book[next].move.after].position.score;
		int nag= book[book[next].move.after].position.nag;
		if (omit_bad && (nag == 2 || (nag >= 4 && nag < 7)))
			score= 0;
		if (score < 0)
			score= 0;
		ends[index]= score;
		if (index)  ends[index] += ends[index-1];
		next= book[next].move.cnext;
		++index;
	}
	assert (index == count);
	int move= -1;
	if (ends[index-1])  
	{
		Randomize(); 
		int x= rand() % ends[index-1];
		next= book().current;
		for (int i= 0;  i < count; ++i)
		{
			if (x < ends[i])  
			{
				move= i;
				break;
			}
		}
	}
	else 
		move= 0;
	assert (move >= 0);

	/* Return move */
	next= book().current;
	while (move)
	{
		next= book[next].move.cnext;
		--move;
	}
	small_move= book[next].move.move;
	return true; 
}

String Opening_Book::Name(move_t small_move, int op_flags, bool ignore_detail) const
{
	Verbose_Function();

	assert (Is());
	Verbose_Int(has);
	if (! has)  return "";
	Book::usigf_t move= book.Get_Move(small_move, ignore_detail);
	Verbose_Int(move);
	if (!move)  return "";
	Book::usigf_t comment=
		book[book[move].move.after].position.comment;
	Verbose_Int(comment); 
	if (!comment)  return _("(in book)"); 
	String ret= book.Get_String(comment);
	const char *p= ret();
	while (isspace(*p) || *p == '*' || *p == '+' || *p == '[')  
	{
		if (*p == '[')
		{
			++p;
			while (*p && *p != ']')
				++p;
			if (*p)
				++p;
		}
		else
			++p;
	}
	String n= p;
	return Decode(n, op_flags);
}

String Opening_Book::Decode(String comment, int op_flags)
{
	Vector <String> alternatives;
	const char *p= comment();
	while (p < comment()+*comment)
	{
		const char *g= p;
		int paren_depth= 0;
		while (*g && (*g != ';' || paren_depth))  
		{
			if (*g == '(')  ++paren_depth;
			else if (*g == ')') --paren_depth;
			paren_depth= std::max(paren_depth, 0);
//			paren_depth >?= 0;
			++g;
		}
		if (alternatives.Set(*alternatives+1))
		{
			const char *q= g;
			while (isspace(q[-1])) --q;
			alternatives[-1]= String(p, q-p);
		}
		if (! (op_flags & op_alternative)) break;
		p= g;
		if (*p == ';')  ++p;
	}

	String ret;
	for (int i= 0;  i < *alternatives;  ++i)
	{
		if (ret)  ret += ", ";
		ret += Decode(alternatives[i]);
		if (i==0)
		{
			const char *p= ret();
			while (*p && *p != ';')  ++p;
			if (*p)
			{
				if (! (op_flags & op_alternative))
					ret= String(ret(), p-ret());
				else
					ret.c()[p-ret()]= ',';
			}
		}
	}
	return ret; 
}

String Opening_Book::Decode(String comment)
{
	Verbose("Opening_Book::Decode(\"%s\")",
		comment());

	const char *p= comment();
	while (isspace(*p))  ++p;
	const char *q= p;
	while (*q && *q != '(')   ++q;
	if (!*q)  
	{
		Verbose("\tno ( in comment");
		return comment;
	}

	const char *r= q+1;
	int depth= 1;
	while (depth)
	{
		switch (*r++)
		{
		case '\0':  
			Verbose("\t""no matching ( at depth %d", depth);
			return comment;
		case '(':   ++depth;  break;
		case ')':   --depth;  break;
		}
	}
	String arg(q+1, r-q-2);
	if (*p == '_')
	{
		Verbose("\ttranslating \"%s\"", arg()); 
		return _(arg());
	}
	Verbose("\t""arg= \"%s\"", arg());
	String arg_text= Decode(arg);
	Verbose("\t""decoded to \"%s\"", arg_text());
	while (q > p && isspace(q[-1])) --q;
	String function(p, q-p);
	Verbose("\t""function= \"%s\"", function());
	const char *text= "%s";

	static const char *functions[][2]= 
		{
			{"A",  N_("%s Attack"     )},
			{"D",  N_("%s Defense"    )},
			{"E",  N_("%s Move"       )},
			{"F",  N_("%s Formation"  )},
			{"G",  N_("%s Gambit"     )},
			{"L",  N_("%s Line"       )},
			{"M",  N_("%s Mate"       )},
			{"O",  N_("%s Opening"    )},
			{"S",  N_("%s System"     )},
			{"T",  N_("%s Trap"       )},
			{"V",  N_("%s Variation"  )},

			{"a",  N_("Anti-%s"       )},
			{"b",  N_("Classical %s"  )},
			{"c",  N_("Counter-%s"    )},
			{"d",  N_("%s Deferred"   )},
			{"e",  N_("Semi-%s"       )},
			{"f",  N_("Delayed %s"    )},
			{"g",  N_("Modern %s"     )},
			{"l",  N_("Closed %s"     )},
			{"m",  N_("Primitive %s"     )},
			{"n",  N_("Neo-%s"        )},
			{"o",  N_("Open %s"          )},
			{"p",  N_("Pseudo-%s"        )},
			{"r",  N_("%s Reversed"   )},
			{"v",  N_("Improved %s"      )},
			{"x",  N_("Accelerated %s")},
			{"y",  N_("Old %s"           )},

			{"Ac", N_("%s Counterattack" )},
			{"Gc", N_("%s Countergambit" )},
		};
	for (int i= 0; i < (int)(sizeof(functions)/(2*sizeof(const char *))); ++i)
		if (function == functions[i][0])
		{
			text= functions[i][1];
			Verbose("\t""replacement text = \"%s\"",
				text);
			break;
		}

	/* If the %s text begins with #, replace all space by dashes in
	 * the resulting string.  Used for German, in which compound
	 * names are used, e.g. "Grand Prix gambit" -> "Grand-Prix-Gambit"
	 */

	const char *translated= _(text);
	Verbose("\t""translated = \"%s\"", translated);
	bool replace= false;

	if (translated[0] == '#')  
	{
		replace= true;
		++translated;
	}

	String ret= Format(translated, arg_text());

	if (replace)
		for (int i= 0; i < (int)*ret;  ++i)
			if (ret[i] == ' ')
				ret.c()[i]= '-';
	
	return ret;
}
