#ifndef CONTAINING_HH
#define CONTAINING_HH

#include "widget.hh"

template <class Container,   
	  class Contained>   
class Containing  
	:  public Container
{
public:

	class Open_Info
	{
	public:

		Open_Info() {}
		Open_Info(typename Container::Open_Info oi): container_open_info(oi) {}
		Open_Info(typename Contained::Open_Info oi): contained_open_info(oi) {}
		Open_Info(typename Container::Open_Info container_oi,
			  typename Contained::Open_Info contained_oi)
			:  container_open_info(container_oi), contained_open_info(contained_oi)
			{}

		typename Container::Open_Info container_open_info;
		typename Contained::Open_Info contained_open_info;
	};

	bool Open(Pointer <Widget> parent)
		{
			if (! Container::Open(parent))
				return false;

			if (! contained.Open(this))
			{
				Container::Close();
				return false;
			}
			return true;
		}

	bool Open(Pointer <Widget> parent, 
		  typename Contained::Open_Info ui)
		{
			if (! Container::Open(parent))
				return false;
			if (! contained.Open(this, ui))
			{
				Container::Close();
				return false;
			}
			return true;
		}

	bool Open(Pointer <Widget> parent,
		  typename Container::Open_Info ti, 
		  typename Contained::Open_Info ui)
		{
			if (! Container::Open(parent, ti))
				return false;
			if (! contained.Open(this, ui))
			{
				Container::Close();
				return false;
			}
			return true;
		}

	bool Open(Pointer <Widget> parent,
		  typename Container::Open_Info ti)
		{
			if (! Container::Open(parent, ti))
				return false;
			if (! contained.Open(this))
			{
				Container::Close();
				return false;
			}
			return true;
		}

	bool Open(Pointer <Widget> parent,
		  typename Containing <Container, Contained> ::Open_Info oi);

	Contained &operator () () 
		{  return contained;   }

	const Contained &operator () () const
		{ return contained;    }
	Contained *operator -> () 
		{ return & contained; }
	const Contained *operator -> () const
		{ return & contained; }

private:
	Contained contained;

	void Wdg_Append_Child(Widget &widget)
		{
			Container::Wdg_Append_Child(widget); 
		}
};

template <class Type_Containing, class Type_Contained>
bool Containing <Type_Containing, Type_Contained> ::
Open(Pointer <Widget> parent,
     typename Containing <Type_Containing, Type_Contained> ::Open_Info oi)
{
	if (! Type_Containing::Open(parent, oi.container_open_info))
		return false;
	if (! contained.Open(this, oi.contained_open_info))
	{
		Type_Containing::Close();
		return false;
	}
	return true;
}

#endif /* ! CONTAINING_HH */
