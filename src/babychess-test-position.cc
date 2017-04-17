/* All positions in test.fen should be correct.  Test it. 
 */

#include "main.hh"
#include "stream.hh"
#include "read_line.hh"
#include "position.hh"
#include "generator.hh"
#include "notation.hh"
#include "fen.hh"
#include "verbose.hh"
#include "error.hh"

/* Return 0 on success  */
int Test(const char *filename)
{
	printf("Testing positions from %s", filename); 

	Stream stream;
	if (! stream.Open(filename, "r"))
	{
		fprintf(stderr, "cannot open file \"%s\":  %s\n",
			filename, Error::Get()());
		return 1;
	}

	int result= 0;

	String line;
	while (Read_Line(stream(), line))
	{
		putchar('.');
		if (!line.Is())  continue;

		/* LINE is a FEN string */ 

		Position position;
		if (! Parse_FEN(line, position))
		{
			result |= 2;
			continue;
		}
		Generator generator;
		generator.New(position);
		for (int i= 0;  i < *generator;  ++i)
		{
			Position p(position);
			p << generator[i];
			if ((p.In_Check() != 0) != (generator.Get_Move_Name(i).check != 0))
			{
				result |= 4;
			}
			p >> generator[i];
			if (p != position)
			{
				result |= 8;
			}
			String fen= FEN(p);
			if (fen != line)
			{
				result |= 16;
			}
		}
	}
	putchar('\n');

	stream.Close();

	return result; 
}

int Main(int c, char **v)
{
	int result= 0;
	if (c == 1)
		result |= Test("share/test_position/test.fen");
	else
		for (int i= 1;  i < c;  ++i)
		{
			printf("Positions from \"%s\":\n", v[i]);
			result |= Test(v[i]);
		}
	if (result)   
		fprintf(stderr, "Test failed with code %d.\n", result);
	return result;
}
