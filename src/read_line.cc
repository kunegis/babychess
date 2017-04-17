#include "read_line.hh"

#include "stream.hh"
#include "stdio.hh"

bool Read_Line(FILE *file,
	       String &line,
	       bool *end_of_line)
{

#ifdef READ_LINE_GETLINE

	/* This implementation would be preferable but does not work
	 * in conjunction with IO_CHILD_PROCESS for unknown reasons.  
	 */

	line.Free(); 

	size_t n= 0;
	char *lineptr= NULL;
	
	ssize_t g= getline(&lineptr, &n, file);

	if (g < 0)
	{
		Dbg("\terror %d %s",
		    errno, strerror(errno));
		return false;
	}

	assert (g > 0);

	if (g > 1)
		Object_Reset(String, &line, lineptr, g-1);

	free(lineptr); 

	return true; 

#else /* ! READ_LINE_GETLINE */ 

	enum {buf_size= 0x60};
	line.Free();
	char buffer[buf_size];

	while (true)
	{
		if (NULL == stdio_fgets(buffer, buf_size, file))
		{
			if (end_of_line)  *end_of_line= false;
			return line.Is(); 
		}

		assert (buffer[0] != '\0');

		size_t len= strlen(buffer);
		if (len && buffer[len-1] == '\n')
		{
			buffer[len-1]= '\0';
			line.Append_To_Any(buffer, len-1);
			if (end_of_line)  *end_of_line= true; 
			return true;
		}
		else
			line.Append_To_Any(buffer, len);
	}

#endif /* ! READ_LINE_GETLINE */ 

}
