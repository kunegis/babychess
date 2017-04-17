#include "write_text_position.hh"

bool Write_Text_Position(FILE *f, const Position &position)
{
	fprintf (f, "/===============\\\n");
	for (int rank = sq_8; rank >= sq_1; --rank)
	{
		fprintf(f, "|");
		for (int file = sq_a; file <= sq_h; ++file)
		{
			int c= ' ';
			int piece= position[Make_Square(file, rank)];
			if (piece & pi_color)
				c= "PNBRQK"[piece & pi_type];
			fprintf(f, "%c|", c);
		}
		fprintf(f, "\n");
		if (rank != sq_1)
			fprintf(f, "|---------------|\n");
	}
	fprintf(f, "\\===============/\n");
	return ferror(f); 
}

