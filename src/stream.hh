#ifndef STREAM_HH
#define STREAM_HH

/* to do:  use C++ classes */ 

/* Open() opens a named file but a Stream can also be opened as
 * another type of file using functions of other modules such as
 * PIPE.   */

#include "pointer.hh"

#include <stdio.h>

class Stream
	:  public Pointed
{
public:

	Stream()  :  file(NULL)  { }
	~Stream();
	/* Must be closed  */

	bool Is() const 
		{ return file != NULL;  }
	bool Open(const char *filename, 
		  const char *opentype);
	/* Opens the file with name FILENAME.  The Stream object must
	 * be closed. 
	 *
	 * Return value: Wether opening was successfull.
	 */

	bool Close()
	/* Return FALSE on error.  If an error occurs, the object will
	 * be closed, but data was not written or read completely.  */
		{
			assert(file != NULL);

			int fc= fclose(file);
			file= NULL;
			return 0 == fc;
		}

	void Set_FILE(FILE *f)
		{ file= f;  }

	FILE *operator () () const
		{
			assert (file != NULL);
			return file; 
		}

	int FD() const
		{
			assert (file != NULL);
			return fileno(file);
		}

	bool Dup(int fd, const char *opentype);
	/* Stream must be closed.  Duplicate FD.  Return TRUE on
	 * success, FALSE on failure.  MOD is the mode passed to
	 * fdopen().  	 
	 */
	bool Dup(const Stream &stream, const char *opentype)
		{ return Dup(stream.FD(), opentype);  }

private:
	FILE *file;
	/* NULL when closed */
};

#endif /* ! STREAM_HH */
