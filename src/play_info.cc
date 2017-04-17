#include "play_info.hh"

#include <glib/gutils.h>

void Play_Info::Reset()
{
	players[cc_w].type= p_human;
	players[cc_w].engine_conf_name= Engine::Default_Conf_Name();
//	players[cc_w].engine.Reset();
	players[cc_w].name= g_get_real_name(); 
			
	players[cc_b].Reset();

	time= 600;
}
