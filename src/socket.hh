#ifndef SOCKET_HH
#define SOCKET_HH

#include "io_child_process.hh"

class Socket
        :  public IO_Child_Process
{
public:

	bool Open(String host, int port= -1);
};

#endif /* ! SOCKET_HH */
