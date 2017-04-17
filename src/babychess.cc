#include "arg_main.hh"
#include "start_window.hh"
#include "game_editor.hh"
#include "game_archive_editor.hh"
#include "position_editor.hh"
#include "play_window.hh"
#include "internet_window.hh"
#include "error.hh"
#include "inform.hh"

#include "../config.hh"

#include <gtk/gtkmain.h>

template <class Type>
int Gui_Main(int argc, char **argv)
{
	Verbose_Int(argc); 
	int result= 0;

	Type main_window;

	if (main_window.Open(typename Type::Open_Info(argc, argv)))
	{
		gtk_widget_show(main_window.Get()); 

		gtk_signal_connect(GTK_OBJECT(main_window.Get()), "destroy",
				   GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
		gtk_main(); 
	}
	else
	{
		String error= Error::Get();
		String message;
		if (argv[1] == NULL)
		{
			message= error;
		}
		else
		{
			if (argv[1][0] == '-')
			{
				message= error; 
			}
			else
				message= Format(_("Cannot open \"%s\":  %s"),
						argv[1],
						error());
		}
		
		fprintf(stderr,
			"%s:  %s\n",
			program_invocation_short_name, 
			message());
		result= 1;

		if (inform_last != NULL)
		{
			gtk_signal_connect(GTK_OBJECT(inform_last), "destroy",
					   GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
			gtk_main(); 
		}
	}

	return result;
}

int Arg_Main(int argc, char **argv)		
{						
	gtk_init(&argc, &argv);

	/* We want gettext() to always return strings in UTF-8 because GTK+
	 * always needs strings in UTF-8, even if the local encoding is
	 * something else. 
	 */
	bind_textdomain_codeset("babychess", "UTF-8");

	/* Without arguments, show the start window */
	const char *mode= argv[1];
	if (mode == NULL)
	{
		return Gui_Main <Start_Window> (argc, argv);
	}

	/* When the first argument is not an option, consider it a filename.
	 * Select the correct mode (game or position) depending on the file
	 * extension.   
	 */
	if (mode[0] != '-')
	{
		mode= strrchr(mode, '.');
		if (mode)
		{
			if (!memcmp(mode, ".pgn", 5))
				return Gui_Main <Game_Archive_Editor> (argc, argv);
			else if (! memcmp(mode, ".fen", 5))
				return Gui_Main <Position_Editor> (argc, argv); 
				
		}
		return Gui_Main <Game_Archive_Editor> (argc, argv);
	}

	/* Remove the mode option from the arguments */
	memmove(argv+1, argv+2, sizeof (char *) * (argc-1));
	--argc;

	/* Select the correct mode */ 
	if (! strcmp(mode, "--edit-game"))
		return Gui_Main <Game_Editor> (argc, argv);
	else if (! strcmp(mode, "--edit-game-archive"))
		return Gui_Main <Game_Archive_Editor> (argc, argv);
	else if (! strcmp(mode, "--edit-position"))
		return Gui_Main <Position_Editor> (argc, argv);
	else if (! strcmp(mode, "--play"))
		return Gui_Main <Play_Window> (argc, argv);
	else if (! strcmp(mode, "--internet"))
		return Gui_Main <Internet_Window> (argc, argv);
	else 
	{
		bind_textdomain_codeset("babychess", nl_langinfo(CODESET));
		fprintf(stderr,
			"***:  %s:  %s\n",
			_("No such mode"),
			mode);
		return 1;
	}
}

void Arg_Print_Help()
{
	printf(
"%s\n"
"\tbabychess --play\n"
"\t\t%s\n"
"\tbabychess --internet\n"
"\t\t%s\n"
"\n"
"%s"
"\n"
"\tbabychess --edit-game-archive [ %s ]\n"
"\t\t%s\n"
"\tbabychess --edit-game [ %s [ -a ] | -p \"%s\" ]\n"
"\t\t%s\n"
"\tbabychess --edit-position [ %s ]\n"
"\t\t%s\n"
"\n"
"%s"
,
_("BabyChess, a GUI chess application.  Whithout arguments, show\n"
  "the start window giving access to all functionality.  To start\n"
  "in a specific mode, pass the name of the mode as the first argument:\n"),
_("Play a local game of chess"),
_("Play internet chess"),
_("To edit a file, pass the file type and the filename.  The\n"
  "filename is optional.  If not given, open a new object of the\n"
  "specified type:\n"),
_("PGN-FILE"),
_("Edit a PGN file"),
_("SINGLE-PGN-FILE"),
_("FEN-POSITION"),
_("Edit a PGN file that contains just one game"),
_("FEN-FILE"),
_("Edit a FEN file with just one position"),
_("Use -a to open a file anonymously, i.e. don't remember\n"
  "its filename.\n")
		);
}
