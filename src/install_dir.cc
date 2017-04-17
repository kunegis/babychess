#include "install_dir.hh"

#include "filename.hh"
#include "main.hh"
#include "verbose.hh"
#include "format.hh"
#include "strdupa.hh"
#include "home.hh"
#include "../config.hh"

#include <limits.h> 
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

const String &Install_Dir(Install_Dir_Type id)
{
	static String base_dir;
	/* Base installation directory.  Eg: "/usr", "/home/joe"...  */

	if (! base_dir.Is())
	{
		String program_name= program_invocation_name; 

		if (program_name[0] != '/') 
			/* argv[0] is no absolute filename */ 
		{
			/* Try to find out absolute filename */

			/* Read "/proc/PID/exe"  */
			char buffer[PATH_MAX + 1];
			String filename= Format("/proc/%d/exe", (int)getpid()); 
			int r= readlink(filename(), buffer, PATH_MAX);
			if (r != -1)
			{
				program_name= String(buffer, r);
			}
			else
			{
				String filename= "/usr/bin/" + program_name; 
				if (0 == access(filename(), R_OK))
				{
					base_dir= "/usr";
				}
				else
				{
					filename= String(Home()) / "bin" / program_name;
					if (0 == access(filename(), R_OK))
						base_dir= Home(); 
					else
						base_dir= "."; 
				}
			}
		}
		if (! base_dir.Is())
		{
			char *program= strdupa(program_name());
			base_dir= dirname(dirname(program));
		}
	}

	switch (id)
	{
	default:  assert (0);

	case id_base:
		return base_dir;

	case id_bin:
		static String bin_dir;
		if (! bin_dir.Is())
			bin_dir= base_dir / "bin";
		return bin_dir;
		
	case id_files:
		static String files_dir;
		if (! files_dir.Is())
		{
			files_dir= base_dir;
			String baby_dir= files_dir / "share/babychess"; 
			struct stat stat;
			if (0 == ::stat(baby_dir(),&stat) &&S_ISDIR(stat.st_mode))
				files_dir= baby_dir; 
		}

		return files_dir;
	}
}
