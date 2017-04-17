#ifndef PROCESS_OPEN_INFO_HH
#define PROCESS_OPEN_INFO_HH

class Process_Open_Info
{
public:

	/* arguments to a program WITH the program name */ 
	int argc;
	char **argv;

	Process_Open_Info(int c, char **v)
		:  argc(c),  argv(v)
		{ }
};

#endif /* ! PROCESS_OPEN_INFO_HH */ 
