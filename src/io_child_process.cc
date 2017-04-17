#include "io_child_process.hh"

#include "read_line.hh"
#include "error.hh"
//#include "pipe.hh"
#include "verbose.hh"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

bool Pipe(Stream &a, Stream &b)
/* Open a pipe (A for reading, B for writing).  Set line buffering.  Set the
 * close-on-exec-flag. 
 */
{
	assert (! a.Is());
	assert (! b.Is());

	int fd[2];

	if (0 != pipe(fd))  return false;

	FILE *file_a= fdopen(fd[1], "w");
	if (file_a == NULL)
	{
//		int save_errno= errno;
		close(fd[0]);
		close(fd[1]);
//		errno= save_errno;
		return false;
	}

	FILE *file_b= fdopen(fd[0], "r");
	if (file_b == NULL)
	{
//		int save_errno= errno;
		fclose(file_a);
		close(fd[0]);
//		errno= save_errno;
		return false;
	}

	fcntl(fd[0], F_SETFD, FD_CLOEXEC);
	fcntl(fd[1], F_SETFD, FD_CLOEXEC);
	
	setvbuf(file_a, NULL, _IOLBF, 0x80);
	setvbuf(file_b, NULL, _IOLBF, 0x80);

	a.Set_FILE(file_a);
	b.Set_FILE(file_b);

	return true;
}

bool IO_Child_Process::Open(const char *program_filename,
			    const char *args[],
//			    const Vector <String> &args,
			    bool nice)
{
	assert (! Is());
	assert (! (in.Is() || out[0].Is() || out[1].Is()));

	/* Remote end of the pipe.  Closed in this process after the
	 * fork.   */
	Stream in_rem, out_rem, err_rem;

//	if (in    .Is()) in    .Close();
//	if (out[0].Is()) out[0].Close();
//	if (out[1].Is()) out[1].Close();

	if (! (Pipe(in,      in_rem)  &&
	       Pipe(out_rem, out[0])     &&
	       Pipe(err_rem, out[1])))
		return false;

	if (! Child_Process::Open(program_filename,
				  args,
				  in_rem.FD(),
				  out_rem.FD(),
				  err_rem.FD(),
//				  & in_rem,
//				  & out_rem,
//				  & err_rem,
				  nice))
		return false;

#if 0	
	int fd= in.FD();
	int flags= fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif /* 0 */ 

	for (int i= 0; i < 2; ++i)
	{
		int fd= out[i].FD();
		int flags= fcntl(fd, F_GETFL);
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}

	out_buffer[0]= out_buffer[1]= "";

	in_rem.Close();
	out_rem.Close();
	err_rem.Close(); 

	return true;
}

bool IO_Child_Process::Proceed()
{
	Verbose_Function(); 

	assert (in.Is());
	assert (out[0].Is());
	assert (out[1].Is()); 

	/* Even if end-of-file is reached on stdout, stderr must be read. Using
	 * the || operator would shortcut the call to Proceed(1) and miss error
	 * messages. 
	 */

	bool error= Proceed(0);
	error    |= Proceed(1);
	
	return error; 
}

void IO_Child_Process::Write(String line)
{
	Verbose("IO_Child_Process::Write(\"%s\")", line());
	fprintf(Input(), "%s\n", line());
}

void IO_Child_Process::Close(struct rusage *rusage)
{
	in.Close();
	out[0].Close();
	out[1].Close();
	Child_Process::Close(rusage);
}

bool IO_Child_Process::Proceed(int i)
{
	Verbose("IO_Child_Process::Proceed(%d)", i); 

	assert (i >= 0 && i < 2);

	while (true)
	{
		String line;
		bool end_of_line;
		if (Read_Line(out[i](), line, &end_of_line))
		{
			Verbose("\tread \"%s\", end_of_line = %d",
				line(),
				end_of_line);
			if (end_of_line)
			{
				IOCP_Output(i, out_buffer[i] + line);

				/* Be sure that the virtual function did
				 * not close us. */ 
				assert (in.Is());

				out_buffer[i]= "";
			}
			else
				out_buffer[i] += line; 
		}
		else 
		{
			Verbose("\tend reading");
			break;
		}
	}

	if (feof(out[i]()))
	{
		Verbose("\tend of file");
		return true;
	}		

	if (errno != EAGAIN && errno != 0)
	{
		Verbose("\tother error");
		return true;
	}
	Verbose("\tblocked"); 
	return false;
}
