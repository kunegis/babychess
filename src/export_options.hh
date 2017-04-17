#ifndef EXPORT_OPTIONS_HH
#define EXPORT_OPTIONS_HH

#include "pointer.hh"

class Export_Options
	:  public Pointed
{
public:
	enum {t_html, t_text, t_count}; 
	int type; /* t_* */ 

	void Reset()
		{}
};

#endif /* ! EXPORT_OPTIONS_HH */ 
