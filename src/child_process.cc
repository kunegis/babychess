#include "child_process.hh"

#include "filename.hh"
#include "vector.hh"

#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>

Child_Process::~Child_Process()
{
	if (Is())  Close();
}

bool Child_Process::Open(const char *program_filename,
			 const char *args[],
			 int in,
			 int out,
			 int err,
			 bool nice)
{
	assert (! Is());

	if (! Process_Create(program_filename,
			     args,
			     false,
			     & child,
			     in,
			     out,
			     err,
			     nice))
		return false;

	paused= false;

	return true;
}

void Child_Process::Close(struct rusage *rusage)
{
	assert (Is());
	Verbose_Process("kill (%u)", child);

	kill (child, SIGTERM);
	/* The wait below blocks sometimes */ 
	wait4 (child, NULL, 0, rusage);

	child= -1;
}

int Child_Process::Wait()
{
	assert (Is());

	int exit_code;
	if (0 > waitpid(child, &exit_code, 0))
		exit_code= 1;

	child= -1;
	    
	return exit_code;
}

void Child_Process::Pause(bool custom_stop)
{
	assert (Is());

	int sig= paused ? SIGCONT : (custom_stop ? SIGUSR1 : SIGSTOP);
	kill(child, sig);

	paused= ! paused;
}
