#ifndef PLAYER_HH
#define PLAYER_HH

#include "engine.hh"

enum {p_human,
      p_engine,
      p_count};

struct Player
{
	String name;

	int type;

	/* If type == p_engine, the next variable are significant.
	 */
	String engine_conf_name;

	void Reset();
};

#endif /* ! PLAYER_HH */ 
