#ifndef BOX_HH
#define BOX_HH

#include "widget.hh"

class Box
	:  virtual public Widget
{
public:

	struct Open_Info
	{
		int spacing;
		bool homogeneous;  
		bool reverse;

		Open_Info(int s= 0, bool h= false, bool r= false)
			:  spacing(s), homogeneous(h), reverse(r)
			{}
	};

	bool Open(Pointer <Widget> parent,
		  Open_Info= Open_Info());

	bool Open(Pointer <Widget> parent,
		  int spacing)
		{
			Open_Info oi(spacing, false, false);
			return Open(parent, oi);
		}

	void Set_Next(bool resizeable= true,
		      int padding= 0)
		{
			use_next= true;
			next_resizeable= resizeable;
			next_padding= padding;
		}

protected:
	void Wdg_Append_Child(Widget &widget);

private:
	bool next_resizeable;
	int next_padding;
	bool use_next, reverse;


	void Wdg_Get_Size_Request(int & /* width */ ,
				  int & /* height */ ,
				  bool &resizeable);
	virtual GtkWidget *Box_Open(int /* spacing */,
				    bool /* homogenous */ ) 
		const
		{
			return NULL; 
		}
};

class V_Box
	:  public Box
{
private:
	GtkWidget *Box_Open(int spacing, bool homogeneous) const;
};

class H_Box
	:  public Box
{
private:
	GtkWidget *Box_Open(int spacing, bool homogeneous) const;
};

class Table
	:  public Widget
{
public:

	typedef int Open_Info;   /* columns */ 

	bool Open(Pointer <Widget> parent,
		  Open_Info open_info= 2);

	int space;  /* left for high children */
	int width, height; /* of next child */
	bool fill;

private:
	int columns;
	void Wdg_Append_Child(Widget &widget);
};

#endif /* ! BOX_HH */
