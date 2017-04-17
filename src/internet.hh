#ifndef INTERNET_HH
#define INTERNET_HH

/* Client to an internet chess server  */

#include "socket.hh"
#include "position.hh"
#include "message_receiver.hh"
#include "generator.hh"

class Internet
	:  private Socket
{
public:

	Internet() 
		{  position.Reset();  }

	bool Open(String host_name,
		  int port,
		  String user,
		  String passwd);
	bool Is() const 
		/* connection established */ 
		{  return Socket::Is();    }

	bool Inside_Game() const
		{
			assert (Is());
			return inside_game; 
		}

	void Close() 
		{  Socket::Close();  }

	FILE *Output()  {return Socket::Output();	} 
	FILE* Error_Output() {return Socket::Error_Output(); }

	const Position &Current_Position() const 
		{  return position; }

	bool Proceed();
	
	String User() const
		{
			assert (Is());
			return user;
		}

	String Host() const
		{
			assert (Is());
			return host;
		}

	void Seek(int minutes,
		  int secs);
	bool User_Play(int from, int to);
	/* Whether move was played. 
	 */
	int Playing_Color() const
		{
			assert(Is());
			assert(inside_game);
			return observing ? cc_w : play_color;
		}
	String White() const
		{
			assert (Is() && inside_game);
			return white_player;
		}
	String Black() const
		{
			assert (Is() && inside_game);
			return black_player;
		}
	int Result() const
		{
			return result;
		}

	void Resign()
		{
			assert (Is() && inside_game);
			Write("resign");
		}

	void Put(String s) 
		{  Write(s);  }
	String Error() const
		{ return error;  }

	const Generator &Current_Generator() const
		{ return generator;  }

private:

	void IOCP_Output(int i, String line);

	Position position;
	Generator generator; 
	bool ready; /* user & passwd were written */ 
	String host;
	String user;
	String passwd;
	bool seeking;
	/* A game is seeked or must be seeked. 
	 */
	int seek_m, seek_s;
	bool inside_game;
	/* Whether we're currently playing a game. 
	 */
	bool observing;
	String white_player, black_player;
	/* Set when GAME is TRUE. */
	int play_color;

	int result; /* 0=black, 1= draw, 2= white, 3= aborted */
	String error;

	void Write_Seek();
	/* Output the seek command.   */
	int Style_12(const char *p);
	/* Get a style-12 string whithout the "<12>".  Return 0 on
	 * success.   */
	void Write(String, bool echo= true);

	virtual void Internet_Chess_Changed()= 0;
	/* board changed */
	virtual void Internet_Chess_New_Game()= 0;
	virtual void Internet_Chess_Finished()= 0;
	virtual void Internet_Chess_Playing(String move)= 0;
	/* an opponent's move */
	virtual void Internet_Chess_Message(String message)= 0;
	/* Whithout trailing newline. 
	 */
	virtual void Internet_Chess_Clock(int white, int black)= 0;
	/* seconds.  */
};

#endif /* ! INTERNET_HH */
