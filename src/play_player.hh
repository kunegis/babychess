#ifndef PLAY_PLAYER_HH
#define PLAY_PLAYER_HH

#include "xboard_server.hh"
#include "player.hh"
#include "install_dir.hh"
#include "filename.hh"
#include "message_receiver.hh"

#include <memory>

class Play_Player
{
public:
	virtual ~Play_Player() {}
	virtual void MP_Go()= 0;
	virtual void MP_Move(String)= 0;
	virtual void MP_Take_Back()= 0;
	/* If the engine is to play, take back one move.  If the engine is not
	 * to play, take back two moves. 
	 */
};

class Play_Engine
	:  public Play_Player,
	   private XBoard_Server
{
public:
	Play_Engine(const Engine &engine,
		    String new_message,
		    Message_Receiver &new_message_receiver,
		    String starting_position /* FEN */);
	operator bool () const 
		{ return XBoard_Server::operator bool(); }

private:
	Pointer <Message_Receiver> message_receiver;
	String message;

	void MP_Go()
		{ XBoard_Server::Go();  }
	void MP_Move(String move)
		{
			XBoard_Server::Play_Move(move); 
		}
	void MP_Take_Back();
	
	void XS_Move(String move)
		{
			message_receiver->MR_Receive(message + " " + move);
		}
};

/* Return created engine in first arg.
 */
bool New_Play_Player(std::auto_ptr <Play_Player> &,
		     const Player &player,
		     String message,
		     Message_Receiver &,
		     String starting_position /* FEN */ );

#endif /* ! PLAY_PLAYER_HH */ 

