#include "line_reader.hh"

#if ! HAVE_GETLINE
bool Line_Reader::Read (bool dont_remove_newline)
{
	line= "";
	enum {size= 0x80};
	char buf[size];
	while (fgets(buf, size, file))
	{
		int l= 
#if HAVE_STRNLEN
			strnlen(buf, size-1)
#else
			strlen(buf)
#endif
			;

		assert (l > 0);

		line += buf;

		if (buf[l-1] == '\n')   break;
	}

	if (! dont_remove_newline)
	{
		line.Set_Length(*line-1);
	}

	return true;
}

#endif /* ! HAVE_GETLINE */
