#ifndef PLAY_INFO_HH
#define PLAY_INFO_HH

#include "player.hh"
#include "piece.hh"
#include "position.hh"

#include <memory>

class Play_Info
	:  public Pointed
{
public:

	Player players[2]; 

	int time;
	/* Seconds for whole game */

	String starting_position; /* FEN */ 

	void Reset();
};

#endif /* ! PLAY_INFO_HH */
