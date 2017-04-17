#include "socket.hh"

/* Super-cheap implementation of a socket using telnet. 
 */

#include "format.hh"
#include "error.hh"

#include <errno.h>

bool Socket::Open(String host, int port)
{
	if (! host.Is())
	{
		errno= EINVAL; 
		return false;
	}

//	Vector <String> args;
//	if (! args.Set(1))
//		return false;
//	args[0]= host();

	const char *args[]= {NULL, host(), NULL, NULL};
	char buf[0x20];

	if (port >= 0)
	{
//		if (! args.Set(2))
//			return false;
//		args[1]= Format("%d", port); 
		sprintf(buf, "%d", port);
		args[2]= buf;
	}
	return IO_Child_Process::Open("telnet", args);
}
