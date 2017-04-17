#ifndef RGB_HH
#define RGB_HH

#include "pointer.hh"

class RGB
	:  public Pointed 
{
public:

	RGB() {}
	RGB(int new_r,
	    int new_g,
	    int new_b)
		:  r(new_r),
	           g(new_g),
	           b(new_b)
		   {}

	int r, g, b; /* / 256 */
	bool operator != (RGB rgb) const
		{ return r != rgb.r || g != rgb.g || b != rgb.b; }
	bool operator == (RGB rgb) const
		{ return r == rgb.r && g == rgb.g && b == rgb.b; }
};

#endif /* ! RGB_HH */
