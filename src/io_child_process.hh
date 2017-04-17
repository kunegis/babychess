#ifndef IO_CHILD_PROCESS_HH
#define IO_CHILD_PROCESS_HH

/* Child process with built-in line-wise redirection of IO 
 */
/* "Input" and "output" are from the viewpoint of the child process.  
 */

/* Proceed() must be called when data is available on output or error
 * output. 
 */

#include "child_process.hh"
#include "stream.hh"

class IO_Child_Process
	:  public Child_Process
{
public:

	virtual ~IO_Child_Process()   
		{ 
			if (Is())
			{
				in.Close();
				out[0].Close();
				out[1].Close(); 
			}
		}


	bool Open(const char *program_name,
		  const char *args[],
//		  const Vector <String> &args,
		  bool nice= false);
	/* Must be closed */

	FILE *Input()        {  return in();  }
	FILE *Output()       {  return out[0]();  }
	FILE *Error_Output() {  return out[1]();  }

	/* The child is finished when end-of-file is encountered on
	 * one of the outputs.  All files are set to non-blocking.  If
	 * encoutering EOF on reading, call Close().  */
	void Close(struct rusage * = NULL);


	bool Proceed();
	/* Return TRUE if child process has finished, but don't close
	 * if finished.  Call the virtual functions. 
	 */

	void Write(String line);
	/* Write a line to STDIN of process.  '\n' is added to LINE. 
	 */

private:

	Stream in;
	Stream out[2];
	/* All set to non-blocking when opened.   */

	String out_buffer[2];

	/* Whithout the terminating new-line.  A line was read.
	 * Called by Proceed().  The lines may be empty.  May not call
	 * Close().  
	 */
	virtual void IOCP_Output(int /* i */ , String /* line */ ) 
		{ }

	bool Proceed(int);
	/* Return whether the stream must be closed.  */
};

#endif /* ! IO_CHILD_PROCESS_HH */
