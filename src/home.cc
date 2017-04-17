#include "home.hh"

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

const char *Home()
{
	static const char *ret= NULL;
	if (ret == NULL)
	{
		ret= getenv("HOME");
		/* This variable is never removed from the environment */ 

		if (ret == NULL)
		{
			struct passwd *user_passwd= getpwuid(getuid());
			if (user_passwd)
			{
				ret= user_passwd->pw_dir; 
			}
		}
	}
	return ret; 
}
