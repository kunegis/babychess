#include "settings.hh"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <glib/gstrfuncs.h>

const char *Settings_Dir()
{
	static char *dir= NULL;
	if (dir == NULL)
	{
		dir= g_strdup_printf("%s/"SETTINGS_DIR_NAME, g_get_home_dir());
		if (dir == NULL)
			return NULL;
		if (access (dir, F_OK))
		{
			if (0 > mkdir (dir, 0700))
			{
				free(dir);
				return NULL;
			}
		}
	}
	return dir;
}

const char *Settings_Tmp()
{
	static char *dir= NULL;
	if (dir == NULL)
	{
		dir= g_strdup_printf("%s/tmp", Settings_Dir());
		if (dir == NULL)  return NULL;
		if (access (dir, F_OK))
		{
			if (0 > mkdir (dir, 0700))
			{
				free (dir);
				return NULL;
			}
		}
	}
	return dir; 
}
