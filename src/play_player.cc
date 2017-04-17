#include "play_player.hh"

#include "engine_editor.hh"

void Play_Engine::MP_Take_Back()
{
	Take_Back();
}

bool New_Play_Player(std::auto_ptr <Play_Player> &dynamic,
		     const Player &player,
		     String message,
		     Message_Receiver &mr,
		     String starting_position)
{
	switch (player.type)
	{
	default:  assert (0);
	case p_engine:
		std::auto_ptr <Play_Engine> engine;
		Engine e;
		String filename= Install_Dir();
		filename= filename / "share";
		const char *type_name= "engine";
		filename= filename / type_name; 
		filename= filename / player.engine_conf_name;
		if (! Read_From_File(filename(), e))
		{
			filename= String(Settings_Dir())/
				"engine"/
				player.engine_conf_name;
			if (! Read_From_File(filename(), e))
				e.Reset();
		}
		engine.reset(new Play_Engine(e, message, mr, starting_position));
		if (! *engine)
			return false;
		dynamic= engine;
		return true; 
	}
}

Play_Engine::Play_Engine(const Engine &engine,
			 String new_message,
			 Message_Receiver &new_message_receiver,
			 String starting_position)
{
	message= new_message;
	message_receiver= new_message_receiver; 
	String program= engine.program;
	if (engine.type == et_babychess)
		program= Install_Dir(id_bin)+"/babychess-engine";
	XBoard_Server::Open(program(), 0, starting_position);
}
