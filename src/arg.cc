#include "arg.hh"

#include "version_number.hh"
#include "format.hh"
#include "gettext.hh"
#include "../config.hh"

#include <stdio.h>
#include <langinfo.h>
#include <libintl.h>

void Arg(int argc, char **argv)
{
	if (argc > 1)
	{
		/* --help */
		if (0 == strcmp(argv[1], "--help"))
		{
			bind_textdomain_codeset("babychess", nl_langinfo(CODESET));
			Arg_Print_Help();
			String e_mail= Format(_("Send bug reports to %s"), E_MAIL);
			String www=    Format(_("Visit the website at %s"), WEBSITE);
			printf("\n"
			       "%s\n"
			       "\t--help      %s\n"
			       "\t--version   %s\n"
			       "\t -V\n"
			       "\n"
			       "%s\n"
			       "%s\n",
			       _("Meta-options:"),
			       _("Print a short help"),
			       _("Print program version number"),
			       e_mail(),
			       www());
			exit(0);
		}

		/* --version */
		else if (0 == strcmp(argv[1], "--version") ||
			 0 == strcmp(argv[1], "-V"))
		{
			bind_textdomain_codeset("babychess", 
						nl_langinfo(CODESET));
			printf("BabyChess " VERSION_NUMBER "\n");
#ifndef NDEBUG
			printf(_("Debug version (NDEBUG not defined)\n"));
#endif /* ! NDEBUG */
			printf("CXXFLAGS = %s\n", CXXFLAGS);
			printf(_("Compiled %s"), Version_Date());
			printf("\n"
			       "%s\n"
			       "%s\n"
			       "%s\n",
			       _(COPYRIGHT_LINE),
			       _("BabyChess is free software and comes with no warranty."),
			       _("Conceived and developed by Jérôme KUNEGIS."));

			exit(0);
		}
	}
}
