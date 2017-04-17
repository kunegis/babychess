#include "stream.hh"

#include <unistd.h>

bool Stream::Open(const char *filename, const char *opentype)
{
	assert(file == NULL);

	file= fopen(filename, opentype);

	if (file == NULL)
		return false;

	return true;
}

bool Stream::Dup(int fd, const char *opentype)
{
	assert (fd >= 0);
	assert (file == NULL);
  
	int new_fd= dup(fd);

	if (new_fd < 0)
		return false;

	file= fdopen(new_fd, opentype);

	if (file == NULL)
	{
		close(new_fd);
		return false;
	}

	return true;
}

Stream::~Stream()
{
	assert (file == NULL);
	/* Just closing the file here doesn't work.  fclose() may report an
	 * error which would not be noticed.  Always call Close() and check the
	 * return value. 
	 */

	if (file != NULL)  Close();
}
