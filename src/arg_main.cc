#include "arg_main.hh"

#include "main.hh"
#include "arg.hh"
#include "verbose.hh"
#include "install_dir.hh"
#include "format.hh"
#include "filename.hh"

#include <libintl.h>
#include <locale.h>
#include <sys/stat.h>

int Main(int argc, char **argv)
{
	/* Setup gettext().  By default, gettext() outputs strings in the
	 * codeset of the current locale.  However, GTK+ needs all strings in
	 * UTF-8.  Therefore we switch to UTF-8 in src/babychess.cc. 
	 */ 

	setlocale(LC_ALL, "");
	/* Note:  GTK+ might also call this; but not all programs
	 * (e.g. babychess-evaluator) use GTK+.  */

	String dir= Install_Dir(id_base) + "/share/locale";
	struct stat stat;
	if (!(!::stat(dir(), &stat) && S_ISDIR(stat.st_mode)))
	{
		dir= Install_Dir(id_base) + "/po"; 
	}
	bindtextdomain ("babychess", dir());
	textdomain ("babychess");

	Arg(argc, argv); 

	return Arg_Main(argc, argv);
}
