#include "message_receiver.hh"

#include "vector.hh"

#include <glib/gtypes.h>
#include <gtk/gtkmain.h>

class Message
{
public:
	
	Pointer <Message_Receiver> receiver;
	String message; 
};

static Vector <Message> message_receiver_messages;
static bool message_receiver_idle_is= false;
static guint message_receiver_idle_id;

gint Message_Receiver_Idle(gpointer)
{
	assert (message_receiver_idle_is);
	assert (*message_receiver_messages);

	if (message_receiver_messages[0].receiver.Is())
		message_receiver_messages[0].receiver()
			.MR_Receive(message_receiver_messages[0].message);
	for (int i= 0;  i + 1 < *message_receiver_messages;  ++i)
		message_receiver_messages[i]= message_receiver_messages[i + 1];
	message_receiver_messages= -1;
	if (! *message_receiver_messages)
	{
		message_receiver_idle_is= false;
		gtk_idle_remove(message_receiver_idle_id);
	}
	return TRUE;
}

void Message_Receiver::MR_Send(String message) const
{
	if (! message_receiver_idle_is)
	{
		message_receiver_idle_is= true;
		message_receiver_idle_id=
			gtk_idle_add_priority
			(GTK_PRIORITY_HIGH,
			 & Message_Receiver_Idle, 
			 NULL);
	}

	if (! message_receiver_messages.Set(*message_receiver_messages + 1))
		return;

	message_receiver_messages[-1].receiver= (Message_Receiver *)this;
	message_receiver_messages[-1].message= message; 
}

void Message_Receiver::Remove_Pending_Messages()
{
	for (int i= 0;  i < *message_receiver_messages;  ++i)
		if (message_receiver_messages[i].receiver == this)
			message_receiver_messages[i].receiver= NULL; 
}
