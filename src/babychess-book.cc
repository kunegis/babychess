#include "arg_main.hh"
#include "book.hh"
#include "gettext.hh"
#include "dump_book.hh"
#include "game_stream.hh"
#include "stream_file.hh"
#include "format.hh"
#include "fatal.hh"
#include "pgn.hh"
#include "error.hh"
#include "book_scores.hh"
#include "game_text.hh"

#include <unistd.h>

int Arg_Main(int argc, char **argv)
{
	Book book;

	int c;
	while ((c= getopt(argc, argv, "c:r:dDphm:aP")) >= 0)
	{
		switch (c)
		{
		case 'c': /* create new */ 
			if (! book.Open_New(optarg))
				Fatal(Format(_("Creating \"%s\""), optarg));
			break;
		case 'r': /* open read only */ 
			if (! book.Open_Read(optarg))
				Fatal(Format(_("Opening \"%s\""), optarg));
			break;
		case 'a':
			if (! book.Open_Anonymous())
				Fatal(_("Opening an anonymous book"));
			break;
		case 'd': /* dump as text */ 
			if (! book.Is())
			{
				Error::Set(_("No book loaded")); 
				Fatal(_("Dumping"));
			}
			Dump_Book(book);
			break;
		case 'D': /* dump PGN */ 
		{
			if (! book.Is())
			{
				Error::Set(_("No book loaded")); 
				Fatal(_("Dumping"));
			}
			Game game;
			game.New();
			book.Merge_Into_Game(game);
			PGN::Write(stdout, game);
			break;
		}
		case 'p': /* Print text */
		{
			if (!Game_Text::Write_HTML(stdout, book))
				Fatal(_("Printing game as text"));
			break;
		}
		case 'h': /* Print HTML */
		{
			if (!Game_Text::Write_HTML(stdout, book))
				Fatal(_("Printing game as HTML"));
			break;
		}
		case 'm': /* merge PGN */ 
		{
			if (! book.Is())
			{
				Error::Set(_("No book loaded")); 
				Fatal(_("Merging PGN"));
			}

			Game game;
			if (! (Read_From_File(optarg, game) 
			       && book.Merge(game)))
				Fatal(Format(_("Reading \"%s\""), optarg));

			break;
		}
		case 'P': /* set scores */ 
			Set_Book_Scores(book); 
			break;
		}
	}

	if (book.Is())
		book.Close();
	else
		printf(_("No book was loaded.  Try using --help.\n"));

	return 0;
}

void Arg_Print_Help()
{
	printf("%s.\n"
	       "\n"
	       "%s\n"
	       "\n"
	       "%s:\n"
	       "\t-c %10s   %s\n"
	       "\t-r %10s   %s\n"
	       "\t-a              %s\n"
	       "\t-d              %s\n"
	       "\t-D              %s\n"
	       "\t-p              %s\n"
	       "\t-h              %s\n"
	       "\t-m %10s   %s\n"
	       "\t-P              %s\n",
	       _("Manipulate BabyChess books"),
	       _("Open or create a BabyChess book, and execute commands."),
	       _("Commands"),
	       _("FILE"),
	       _("Create a new book"),
	       _("FILE"),
	       _("Open a book for reading only"),
	       _("Create an anonymous book"),
	       _("Dump book content"),
	       _("Print the book as PGN"),
	       _("Print the book as text"),
	       _("Print the book as HTML"),
	       _("FILE"),
	       _("Merge a PGN file"),
	       _("Set the scores of all positions to the number of subsequent positions"));
}
