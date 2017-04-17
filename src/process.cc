#include "process.hh"

#include "error.hh"
#include "format.hh"
#include "filename.hh"
#include "main.hh"
//#include "stream.hh" /* to set the hook */ 

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h> /* basename(3) on Solaris */ 
#include <fcntl.h>

/* If we get a SIGTERM, send it also to our child.  We assume there's
 * only one child at any moment. 
 */
pid_t process_last_pid= -1;

void Process_Term(int)
{
//	Verbose_Process("terminating");
//	Verbose("\tkill (%u)", process_last_pid);
	if (process_last_pid >= 0)   
		kill (process_last_pid, SIGTERM); 
	exit (128+SIGTERM);
}

bool Process_Create(const char *program_filename, 
		    const char *const args[],
//		    const Vector <String> &args_vector,
		    bool wait,
		    pid_t *process,
		    int input,
		    int output,
		    int error_output,
		    bool nice)
{
	assert (wait || process != NULL);
	assert (program_filename);

#ifndef NDEBUG
	Verbose_Process("Process_Create(");
	Verbose("\tprogram_filename = \"%s\",", program_filename); 
	Verbose("\twait = %d,", wait);
	Verbose("\tnice = %d,", nice);
//	if (!*args_vector)
//		Verbose("\t*args = %d,", *args_vector);
	for (const char *const *p= args+1; *p != NULL; ++p)
//	for (int i= 0;  i < *args_vector; ++i)
		Verbose("\targs[%d] = \"%s\",", p-args, *p); 
	Verbose("\t)");
#endif /* ! NDEBUG */ 


	/* First check if PROGRAM is executable.  If it is not, only the child
	 * process would know, and this function would return TRUE.
	 * Process_Create() should return false if the program doesn't exist.
	 * Relative filenames cannot be checked.   */

	if (program_filename[0] == '/')
	{
		if (access(program_filename, X_OK))
		{
			Error::Set_Filename(program_filename); 
			return false;
		}
	}
	
	signal(SIGTERM, &Process_Term);

	pid_t pid= fork();
	
	switch (pid)
	{
	case -1: /* error */
		return false;

	case 0: /* child */
	{
		/* Set nice */
#ifndef PRIO_MAX
#    ifdef NZERO
#        define PRIO_MAX ((2 * NZERO) - 1)
#    else
#        define PRIO_MAX 19
#    endif
#endif
		if (nice)  ::nice(PRIO_MAX);

		/* redirect in/output */
		if (input        >= 0) dup2(input,        0);
		if (output       >= 0) dup2(output,       1);
		if (error_output >= 0) dup2(error_output, 2);

#if 0
		/* Build argument list  */
		const char *args[*args_vector + 2];
		for (int i= 0;  i < *args_vector;  ++i)
			args[i + 1]= args_vector[i]();
		args[*args_vector + 1]= NULL;
#endif /* 0 */ 

		/* execute */ 
		char *s= strdupa(program_filename);
		String base_name= basename(s);
//		free(s);
		((const char **)args)[0]= base_name(); 
		execvp(program_filename, (char *const *)args);
		/* execvp() only returns on error */

		fprintf(stderr,
			"\t[child] *** error:  cannot execute \"%s\":  %s.\n",
			program_filename,
			Error::Get()());

		_exit(1);
		/* We must use "_exit()" instead of "exit()", else destructors
		 * would be called in the forked process.   */
		return false;
	}

	default: /* parent */
		Verbose("\t=%u", pid);
		process_last_pid= pid; 
		if (wait)
			waitpid(pid, NULL, 0);
		else
		{
			*process= pid;
		}

		return true;
	}
}
