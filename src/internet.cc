#include "internet.hh"

#include "format.hh"
#include "generator.hh"
#include "verbose.hh"
#include "notation.hh"
#include "gettext.hh"

#include <gdk/gdk.h>
#include <ctype.h>

/* Static helper functions.  to be replaced by library functions. 
 */
static bool Compare_Words(const char *s0, const char *s1)
{
	assert(s0 && s1);

	while (!isspace(*s0) && *s0 && *s0==*s1)
	{++s0; ++s1; }

	return (isspace(*s0) || !*s0) && (isspace(*s1) || !*s1);
}

static int Get_Next_Word(const char *c)
{
	assert (c != NULL);
	const char *d= c;
	while (*d && !isspace(*d))
		++d;
	while (isspace(*d))
		++d;
	return d-c;
}

static int Skip_Word(const char *c)
{
	assert (c != NULL);
	const char *d= c;
	while (*d != '\0' && ! isspace(*d))
		++ d;
	return d - c;
}

bool Internet::Open(String host_name,
			  int port,
			  String u,
			  String p)
{
	assert (! Is());

	if (! Socket::Open(host_name, port))
	{
		return false;
	}

	host= host_name;
	user= u;
	passwd= p;
	seeking= false;
	ready= false;
	inside_game= false;
	error= "";

	return true;
}

void Internet::IOCP_Output(int i, String line)
{
	Verbose("Internet::IOCP_Output(i = %d, line = \"%s\")",
		i, line()); 

	Internet_Chess_Message(line);

	if (i)
	{
		if (! error)
		{
			error= line;
			Verbose("\tset error = \"%s\"", error()); 
		}
		return; 
	}

	if (! line.Is())
		return;

	const char *p= line();

	while (isspace(*p))  ++p;

	if (Compare_Words(p, "login:"))
	{
		Write(user);
		Write(passwd, false);
		Internet_Chess_Message(Format("<%s>", _("Sent password")));
		Write("");
		Write("style 12"); 
		/* Set to style #12, as does XBoard. */

		ready= true;
		if (seeking)
			Write_Seek();
	}
	else if (Compare_Words(p, "<12>"))
	{
		p += Get_Next_Word(p);
		Style_12(p);
	}
	else if (p[0] == '{')
	{
		inside_game= false;

		while (p[0] != '}' && p[0] != '\0')
			++ p;
		if (p == '\0')
			return;
		++p;
		while (isspace(*p))  ++p; 
		if (Compare_Words(p, "1-0"))
			result= 2;
		else if (Compare_Words(p, "0-1"))
			result= 0;
		else if (Compare_Words(p, "1/2-1/2"))
			result= 1;
		else
			result= 3;

		Internet_Chess_Finished();
	}
}

void Internet::Seek(int m, int s)
{
	assert (Is());

	seeking= true;
	seek_m= m;
	seek_s= s;

	if (ready)
		Write_Seek();
}

void Internet::Write_Seek()
{
	assert (ready);

	Write(Format("seek %d %d",
		     seek_m,
		     seek_s));
}

int Internet::Style_12(const char *p)
{
	assert (Is());

	Position pos;

	pos.Current_Material().Clear();

	for (int rank= 7; rank >= 0; -- rank)
	{
		for (int file= 0; file < 8; ++ file)
		{
			if (p[file] == '\0')
				return 1;
			if (p[file] == '-')
				pos[Make_Square(file,rank)]= pi_empty;
			else
			{
				int new_pt= San_Char(p[file]);
				if (new_pt == 8)
					return 2;
				int new_cc= p[file] >= 'a' && p[file] <= 'z' ? cc_b : cc_w; 
				pos[Make_Square(file, rank)]= Piece(new_cc, new_pt);

				++ pos.Current_Material()(new_cc, new_pt);
				if (new_pt == pi_K)
				{
					if (pos.Current_Material(new_cc, pi_K) > 1)
						/* more than one king on one side */ 
						return 3;
					pos.Set_King_Square(new_cc, Make_Square(file, rank));
				}
			}
		}
		p += Get_Next_Word(p);
	}

	if (! (pos.Current_Material(cc_w, pi_K) && pos.Current_Material(cc_b, pi_K)))
		return 5;

	if (p[0] == 'B')
		pos.Set_To_Move(cc_b);
	else if (p[0] == 'W')
		pos.Set_To_Move(cc_w);
	else
		return 6;

	p += Get_Next_Word(p);

	if (Compare_Words(p, "-1"))
		pos.EP_Poss()= 0;
	else if (p[0] >= '0' &&
		 p[0] <= '7')
		pos.EP_Poss()= Make_Square
			(p[0] - '0', 
			 sq_5 - pos.To_Move());
	else 
		return 7;

	bool castle[4];
	for (int i= 0; i < 4; ++i)
	{
		p += Get_Next_Word(p);
		if (p[0] == '0')
			castle[i]= false;
		else if (p[0] == '1')
			castle[i]= true;
		else 
			return 8;
	}

	pos.Set_Moved(0, castle[1] ? 0 : 1);
	pos.Set_Moved(1, castle[0] || castle[1] ? 0 : 1);
	pos.Set_Moved(2, castle[0] ? 0 : 1);
	pos.Set_Moved(3, castle[3] ? 0 : 1);
	pos.Set_Moved(4, castle[2] || castle[3] ? 0 : 1);
	pos.Set_Moved(5, castle[2] ? 0 : 1);

	p += Get_Next_Word(p);
	p += Get_Next_Word(p);

	p += Get_Next_Word(p);
	if (p[0] == '\0')
		return 9;

	String white(p, Skip_Word(p));
	p += Get_Next_Word(p);
	if (p[0] == '\0')
		return 10;
	String black(p, Skip_Word(p));
	
	p += Get_Next_Word(p);

	if (p[0] == '\0')
		return 11;

	int my_color;
	bool new_observing= false;
	if (Compare_Words(p, "1"))
		my_color= pos.To_Move();
	else if (Compare_Words(p, "-1"))
		my_color= 1^pos.To_Move();
	else
		new_observing= true;

	for (int i= 0; i < 5; ++i)
	{
		p += Get_Next_Word(p);
		if (p[0] == '\0')
			return 12;
	}

	String white_clock(p, Skip_Word(p));
	p += Get_Next_Word(p);
	if (p[0] == '\0')
		return 13;
	String black_clock(p, Skip_Word(p));
	p += Get_Next_Word(p);
	if (p[0] == '\0')
		return 14;

	int white_time, black_time;
	if (1 != sscanf(white_clock(), "%d", & white_time))
		return 15;
	if (1 != sscanf(black_clock(), "%d", & black_time))
		return 16;

	for (int i= 0; i < 3; ++i)
	{
		p += Get_Next_Word(p);
		if (p[0] == '\0')
			return 17;
	}

	String move= String(p, Skip_Word(p));

	if (new_observing || pos.To_Move() == my_color)
		gdk_beep();

	/* allow non-normalized positions */

	if (inside_game)
	{
		if (white_player != white ||
		    black_player != black)
			inside_game= false;
		else if (new_observing || pos.To_Move() == play_color)
			Internet_Chess_Playing(move);
	}
	if (! pos.Set_Legal())
		return 18; 
	position= pos;
	generator.New(position); 

	if (! inside_game)
		/* begin new game */
	{
		inside_game= true;
		observing= new_observing;
		white_player= white;
		black_player= black;
		play_color= my_color;
		Internet_Chess_New_Game();
	}

	Internet_Chess_Changed();
	Internet_Chess_Clock(white_time, black_time);
	return 0;
}

bool Internet::User_Play(int from, int to)
{
	assert (Is());

	if (inside_game)
	{
		if (play_color != position.To_Move())
			return false;
		Generator generator;
		generator.New(position);
		int sm= generator.Square_Move(from, to);
		if (sm == -1)
		{
			return false;
		}
		else
		{
			char text[5]= {'a'+File(from),
				       '1'+Rank(from),
				       'a'+File(to),
				       '1'+Rank(to),
				       '\0'};
			Write(text); 
			position << generator.Get_Move(sm);
			generator.New(position); 
			Internet_Chess_Changed();
			return true;
		}
	}
	else
		return false;
}

bool Internet::Proceed() 
{
	Verbose_Function();
	return Socket::Proceed(); 
	Verbose("\tend Proceed()");
}

void Internet::Write(String s,
		     bool echo)
{
	Socket::Write(s);
	if (echo)
		Internet_Chess_Message(s);
}
