#ifndef MESSAGE_RECEIVER_HH
#define MESSAGE_RECEIVER_HH

/* A Message_Receiver can receive a command through the "MR_Receive()"
 * function.  The command will arrive later than it was posted. 
 */

#include "pointer.hh"
#include "string.hh"

class Message_Receiver
	:  virtual public Pointed 
{
public:

	virtual void MR_Receive(String /* message */) {}
	void MR_Send(String message) const;
	void Remove_Pending_Messages();
};

#endif /* ! MESSAGE_RECEIVER_HH */
