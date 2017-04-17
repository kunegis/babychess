#include "dir.hh"

#include "verbose.hh"

#include <sys/stat.h>

bool Dir::Open(String directory_name)
{
	assert (! Is()); 
	assert (directory_name.Is()); 

	dir= opendir(directory_name());
	return dir != NULL;
}

bool Dir::Read(String &filename)
{
	if (! Is())  return false; 

	struct dirent *dirent= readdir(dir);
	if (dirent == NULL)
		return false; 

	filename= dirent->d_name;

	return true;
}
