#include "position_stream.hh"

#include "read_line.hh"
//#include "parse_fen.hh"
#include "error.hh"
#include "fen.hh"
#include "stdio.hh"

bool Read_Stream(FILE *file,
		 Position *position)
{
	String line;
	if (! Read_Line(file, line))
		return false;

	if (! line.Is())
		return false;

	if (! Parse_FEN(line, *position))
		return false;
	
	return true;
}

bool Write_Stream(FILE *file,
		  const Position *position)
{
	if (! position->Would_Be_Legal())
		return false;

	String s= FEN(*position);
	return  stdio_put_string(s, file) &&
		0 <= stdio_putc('\n', file); 
}
