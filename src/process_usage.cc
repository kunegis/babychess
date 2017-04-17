#include "process_usage.hh"

#include "format.hh"
#include "read_line.hh"
#include "dir.hh"
#include "stream.hh"

#include "../config.hh"

#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>

#if HAVE_PROCFS
#    include <procfs.h>
#endif

int Process_Usage(pid_t pid)
{
//	Process_Usage process_usage(2, 1, 1, 2, 1, 1); 

	int ret= 1;

	Stream file;
	String filename= Format("/proc/%d/stat", (int)pid);
	if (! file.Open(filename(), "r"))
	{
#if HAVE_PROCFS
		String filename= Format("/proc/%d/status", (int)pid);
		if (! file.Open(filename(), "r"))
			return Process_Usage(2, 1, 1, 2, 1, 1);

		struct pstatus status;
		
		if (1 > fread(&status, sizeof (status), 1, file()))
			return Process_Usage(2, 1, 1, 2, 1, 1);

		int user= status.pr_utime.tv_sec * 1000 + status.pr_utime.tv_nsec / 1000000;
		int sys=  status.pr_stime.tv_sec * 1000 + status.pr_stime.tv_nsec / 1000000;
		int child_user= status.pr_cutime.tv_sec * 1000 + status.pr_cutime.tv_nsec / 1000000;
		int child_sys=  status.pr_cstime.tv_sec * 1000 + status.pr_cstime.tv_nsec / 1000000; 
		process_usage= Process_Usage(user + sys, sys, user,
					     child_user + child_sys, child_sys, child_user); 

#endif /* ! HAVE_PROCFS */		
	}
	else
	{
		String line;
		if (Read_Line(file(), line))
		{
			int pid;
			char comm[256];
			char state;
			int ppid;
			int pgrp;
			int session;
			int tty;
			int tpgid;
			unsigned flags;
			unsigned minflt;
			unsigned cminflt;
			unsigned majflt;
			unsigned cmajflt;
			int utime;
			int stime;
			int cutime, cstime; 

			if (17 == sscanf(line(),
					 "%d %256s %c %d %d %d %d %d %u %u %u %u %u %d %d %d %d",
					 & pid,
					 comm,
					 & state,
					 & ppid,
					 & pgrp,
					 & session,
					 & tty,
					 & tpgid,
					 & flags,
					 & minflt,
					 & cminflt,
					 & majflt,
					 & cmajflt,
					 & utime,
					 & stime,
					 & cutime,
					 & cstime)) 

				ret= 10 * (utime + stime + cstime + cutime);
				
//				process_usage= Process_Usage((utime + stime) * 10, 
//							     stime * 10, 
//							     utime * 10,
//							     (cutime + cstime) * 10,
//							     cstime * 10,
//							     cutime * 10);
		}
	}

	if (file.Is())  file.Close(); 

	return ret; 
}
