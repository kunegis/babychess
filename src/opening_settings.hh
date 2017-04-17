#ifndef OPENING_SETTINGS_HH
#define OPENING_SETTINGS_HH

#include "pointer.hh"

enum {op_subvariation= 1 << 0,
      op_alternative = 1 << 1,

      op_count= 2};

class Opening_Settings
	:  public Pointed
{
public:
	int flags;
	void Reset()
		{  flags= 0;   }
	static const char *Default_Conf_Name()
		{  return NULL;   }
};

#endif /* ! OPENING_SETTINGS_HH */ 
