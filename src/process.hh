#ifndef PROCESS_HH
#define PROCESS_HH

#include "vector.hh"
#include "string.hh"

#include <sys/types.h>

bool
Process_Create(const char            *program_filename, 
	       const char *const      args[],
	       bool                   wait,
	       pid_t                 *process= NULL,
	       int                    input= -1,
	       int                    output= -1,
	       int                    error_output= -1,
	       bool                   nice= false);
/* Create a new process that executes PROGRAM_FILENAME. 
 *
 * [ARGS are the arguments given to the program that is executed.  ]
 * ARGS must contain NULL at the beginning for the program name and be
 * terminated by NULL. 
 *
 * If WAIT is TRUE, the function waits until the process finishes, and does not 
 * write anything into PROCESS.  If WAIT is FALSE, the function returns
 * immediately and writes the process ID into PROCESS.     
 *
 * The return value is TRUE when a process was created.
 *
 * If WAIT is TRUE, PROCESS is ignored.  If WAIT is FALSE, PROCESS must not be
 * NULL.  If WAIT is FALSE and the process is created, the process ID of the
 * newly created process is written into PROCESS.  
 *
 * INPUT, OUTPUT and ERROR_OUTPUT contain, not negative, the FD that the input,
 * output and error output, respectively, are redirected to.  The three open
 * streams are not affected in THIS process.  They must be open when
 * Process_Create() is called, but can be closed right after the call, even if
 * WAIT was FALSE.  
 *
 * If NICE, execute child with low priority. 
 */

/* Note:  Open file descriptors are usually not closed on exec().  The
 * "close-on-exec" flag must be set explicitely when a file will be open for
 * longer.  (IO_CHILD_PROCESS does this) 
 */

extern pid_t process_last_pid;
/* Last started or -1 */ 

#endif /* ! PROCESS_HH */
