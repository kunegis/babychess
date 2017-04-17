#include "filename.hh"

#include "strdupa.hh"

#include <libgen.h>

String operator / (String directory_name, 
		   String filename)
{
	/* If FILENAME is absolute, return it.   */
	if (filename[0] == '/')  return filename;

	/* If DIRECTORY_NAME is empty, return FILENAME.   */
	if (*directory_name == 0)  return filename;

	/* If DIRECTORY_NAME does not end in '/', add it.   */
	if (directory_name[-1] != '/')  directory_name += '/';

	return directory_name + filename;
}
