#ifndef MATERIAL_WIDGET_HH
#define MATERIAL_WIDGET_HH

#include "image_widget.hh"
#include "position.hh"

class Material_Widget
	:  public Image_Widget
{
public:

	class Open_Info {};

	bool Open(Pointer <Widget> parent,
		  Open_Info= Open_Info());

	/* Usage note:  Don't open at the beginning.  Call the next three
	 * functions when appropriate, which also work when closed. 
	 */
	void Set_Turned(const Position &, bool turned);
	void Update(const Position &,
		    bool turned,
		    Pointer <Widget> box);
	void Hide()
		{
			if (Is())  Set_Visible(false); 
		}

private:
	enum {width= dialog_unit*5/3};

	bool turned;
	Position position;
	
	void Image_Widget_Get_Dimension(int &new_width,
					int &new_height); 
	void Image_Widget_Build(Pixmap &image);

	void Set(const Position &new_position,
		 bool new_turned);

	static bool Show(const Position &);
};

#endif /* ! MATERIAL_WIDGET_HH */
