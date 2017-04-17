#ifndef BUTTONED_HH
#define BUTTONED_HH

#include "button.hh"
#include "containing.hh"

template <class Type>
class Buttoned
	:  public Containing <Button, Type>
{
};

#endif /* ! BUTTONED_HH */ 
