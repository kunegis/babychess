#ifndef CHILD_PROCESS_HH
#define CHILD_PROCESS_HH

#include "process.hh"
#include "process_usage.hh"

class Child_Process
{
public:
	Child_Process()
		:  child(-1) {}
	~Child_Process();
	/* Killed on destruction  */

	bool Is() const
		{  return child >= 0;  }
	operator bool () const
		{ return Is(); }

	bool Open(const char *program_filename,
		  const char *args[],
		  int in,
		  int out,
		  int err,
		  bool nice= false);
	/* Must not be. */

	void Close(struct rusage * = NULL);
	/* Must be open.  Put info into RUSAGE if not NULL  */

	int Wait();
	/* Must be open.  Wait for completion and return exit code  */

	void Pause(bool custom_stop= false);
	/* if running pause, if pausing resume  */
	bool Paused() const
		{
			assert (Is());
			return paused;
		}

	int Usage() const
		{
			assert (Is());
			return Process_Usage(child);
		}
	pid_t PID() const
		{
			assert (Is());
			return child; 
		}

private:

	pid_t child;
	/* = -1 when closed. */
	bool paused;
};

#endif /* ! CHILD_PROCESS_HH */
