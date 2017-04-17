#include "engine.hh"

#include "../config.hh"

void Engine::Reset()
{
	type= et_babychess;
	program= "gnuchess"; 
}

String Engine::Name() const
{
	switch (type)
	{
	default:assert (0); 

	case et_babychess:
		return Format("BabyChess %s",
			      VERSION_NUMBER);
	case et_xboard:
		return program; 
	}
}
