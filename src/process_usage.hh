#ifndef PROCESS_USAGE_HH
#define PROCESS_USAGE_HH

/* Should be replaced by direct usage of struct timeval.  
 */

#include "process.hh"

#if 0
class Process_Usage
{
public:

	/* All times in milliseconds. 
	 */

	int total, system, user;
	/* Only the process. 
	 */
	int children_total, children_system, children_user;
	/* Only child processes that have terminated. 
	 */

	Process_Usage(int new_total,
		      int new_system,
		      int new_user,
		      int new_children_total,
		      int new_children_system,
		      int new_children_user)
		:  total(new_total),
		   system(new_system),
		   user(new_user),
		   children_total(new_children_total),
		   children_system(new_children_system),
		   children_user(new_children_user)
	{ }
};
#endif

int Process_Usage(pid_t);
/* milliseconds */

#endif /* ! PROCESS_USAGE_HH */
