#ifndef ENGINE_HH
#define ENGINE_HH

#include "string.hh"
#include "gettext.hh"
#include "format.hh"

enum {et_babychess,
      et_xboard,
      et_count};

class Engine
	:  virtual public Pointed
{
public:
	int type;
	
	/* xboard */
	String program;

	void Reset();
	String Name() const;

	static const char *Default_Conf_Name()
		{ return "GNUChess";  }
};

#endif /* ! ENGINE_HH */ 
