#include "xboard_server.hh"

#include "position.hh"
#include "parser.hh"
#include "fen.hh"

bool XBoard_Server::Open(const char *engine,
			 int new_verbose,
			 String initial_position)
{
	Verbose("XBoard_Server::Open(\"%s\")", engine); 

	/* verbose */ 
	while (*engine == '#')
	{
		++engine;
		++verbose;
	}


	const char *args[]= {NULL, NULL};
	if (! IO_Child_Process::Open(engine, args, true))
		return false;

	verbose= new_verbose;
	mode= m_play; 
	post= false; 
	quit= false;

	output.Open(fileno(Output()));
	error_output.Open(fileno(Error_Output())); 

	/* write initial stuff */ 
	Write("xboard"); 

	/* Set initial position */ 
	if (initial_position)
		Set_FEN(initial_position());

	return true;
}

void XBoard_Server::Input()
{
	if (Proceed())  quit= true; 
}

void XBoard_Server::IOCP_Output(int, String line)
{
	Line(line); 
}

void XBoard_Server::Line(String line)
{
	if (verbose)
		fprintf(stderr, "engine>%s\n", line());
	Verbose("engine>%s", line());

	Parser parser(line);
	parser.Skip_Space();

	/* engine move */ 
	if (parser.Skip("my move is ")  ||
	    parser.Skip("move ")        ||
	    parser.Skip("My move is: "))
	{
		String move= parser.Skip_Word();
		XS_Move(move); 
	}

	/* output of analysis */ 
	else if (isdigit(parser.Next_Char())) 
	{
		unsigned depth;
		if (1 != parser.Skip_Format("%u", &depth))  return;
		parser.Skip_Space();
		int score;
		if (1 != parser.Skip_Format("%d", &score))  return;
		parser.Skip_Space();
		unsigned time;
		if (1 != parser.Skip_Format("%u", &time))  return;
		parser.Skip_Space();
		knots_t knots;
		if (1 != parser.Skip_Format("%"SCNknots, &knots))  return; 
		
		parser.Skip_Space();
		String word= parser.Skip_Word();

		/* is a move number or "...", read the next word */  
		while (isdigit(word[0]) || word[0] == '.') 
		{
			parser.Skip_Space();
			word= parser.Skip_Word();
		}

		/* remove leading "(" if present */ 
		if (word[0] == '(')
		{
			word= word.Sub(1, *word-1);
		}

		/* skip the word beginning with "book" */
		if (!memcmp(word(), "book", 4))
		{
			parser.Skip_Space();
			word= parser.Skip_Word(); 
		}

		if (! word)  return;

		XS_Analyze(depth, score, knots, word);
	}
}

bool XBoard_Server::Set_FEN(const char *fen)
{
	Position position;
	if (! Parse_FEN(fen, position))
		return false;

	if (position.To_Move())
	{
		Write("new");
		Write("force");
		Write("a2a3");
	}
	Write("edit");
	Write("#");
	int color= cc_w;
	for (int file= sq_a;  file <= sq_h;  ++file)
		for (int rank= sq_1;  rank <= sq_8;  ++rank)
		{
			int square= Make_Square(file, rank);
			int piece= position[square];
			if (piece & pi_color)
			{
				if (Piece_Color(piece) ^ color)
				{
					color ^= 1;
					Write("c");
				}
				Write(Format("%c%c%c",
					     "PNBRQK"[piece & pi_type],
					     file + 'a',
					     rank + '1'));
			}
		}
	Write(".");
	
	return true; 
}

void XBoard_Server::Write(String line)
{
	if (verbose)
		fprintf(stderr, "engine<%s\n", line());
	Verbose("engine<%s", line());
	IO_Child_Process::Write(line); 
}
