#include "player.hh"

void Player::Reset()
{
	type= p_engine;
	name= engine_conf_name= Engine::Default_Conf_Name(); 
//	engine.Reset(); 
//	name= engine.Name(); 
}
