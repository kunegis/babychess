#include "board_settings.hh"

#include <errno.h>
#include <assert.h>

bool Read_Stream(FILE *file, Board_Settings *settings)
{
	if (1 != fscanf(file, "%u", & settings->flags))
		return false;
	if (settings->flags < 0 || (settings->flags >> b_count) != 0)
	{
		errno= EINVAL;
		return false;
	}
	return true;
}

bool Write_Stream(FILE *file, const Board_Settings *settings)
{
	assert (settings->flags >= 0 && (settings->flags>>b_count) == 0);
	fprintf(file, "%u", settings->flags);
	return ! ferror(file);
}
