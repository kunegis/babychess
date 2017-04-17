#include "execute.hh"

#include "process.hh"

bool Execute(const char *filename,
//	     Pointer <const Vector <String> > args)
	     const char *const args[])
{
#if 0
#ifndef NDEBUG
	Verbose_Function();
	if (args)
	{
		Verbose("\targs= ");
		for (int i= 0; i < *args();  ++i)
			Verbose("\targs[%d]= \"%s\"",
				i,
				args()[i]());
	}
	else
	{
		Verbose("\targs:  empty");
	}
#endif /* ! NDEBUG */ 
#endif /* 0 */ 

	pid_t process;
//	Vector <String> empty_args;
//	const char *const empty_args[]= {NULL, NULL};
	return Process_Create(filename,
			      args,
//			      args ? args : empty_args,
			      false,
			      & process);
}
