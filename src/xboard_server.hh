#ifndef XBOARD_SERVER_HH
#define XBOARD_SERVER_HH

#include "io_child_process.hh"
#include "format.hh"
#include "input_reader.hh"
#include "knots.hh"

enum {m_play, m_analyze};

class XBoard_Server
	:  public IO_Child_Process,
	   public Pointed
{
public:
	XBoard_Server()
		:  output(this), error_output(this)
		{ }

	bool Open(const char *engine,
		  int verbose= 0,
		  String starting_position= "" /* FEN */);
	/* to indicate verbose mode, '#' may be prepended to the engine
	 * name. */ 
	void Analyze()
		{
			mode= m_analyze;
			Write("analyze");
		}
	void Set_Post(bool new_post= true)
		{
			if (post == new_post)  return; 
			post= new_post;
			Write(post ? "post" : "nopost"); 
		}
	bool Set_FEN(const char * /* fen */ );
	bool Get_Quit() const
		{ return quit; }
	void Go()
		{  Write("go");  }
	void Play_Move(String move)
		{  Write(move);  }
	void Take_Back()
		{
			Write("remove"); 
		}

private:

	class Reader
		:  public Input_Reader
	{
	public:
		Reader(Pointer <XBoard_Server> new_server)
			:  server(new_server)
			{ }
		
	private:

		Pointer <XBoard_Server> server;
		
		virtual void IR_Input()
			{ server->Input(); }
	};

	int verbose;
	int mode;
	bool post; 
	bool quit;

	Reader output, error_output;

	virtual void XS_Analyze(unsigned /* depth */,
				int      /* score */,
				knots_t  /* knots */,
				String   /* main_move */ )
		{}
	virtual void XS_Move(String /* move */ )
		{}

	void Input();
	void IOCP_Output(int, String);
	void Line(String); 
	void Write(String line);
};

#endif /* ! XBOARD_SERVER_HH */ 
