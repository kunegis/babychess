#include "material_widget.hh"

#include "scaled_board_image.hh"

#include <algorithm>

void Material_Widget::Image_Widget_Build(Pixmap &image)
{
	assert (Is());

	int count[6][2];
	/* count of piece that will be displayed. */ 
			
	for (int pt= 0;  pt < 6;  ++pt)
	{
		int smallest= std::min(position.Current_Material(0, pt), position.Current_Material(1, pt));
		count[pt][0]= position.Current_Material(0, pt) - smallest;
		count[pt][1]= position.Current_Material(1, pt) - smallest;
	}

	int number[2];
	/* number of displayed pieces by color */ 

	for (int cc= 0;  cc < 2;  ++cc)
	{
		number[cc]= 0;
		for (int pt= 0;  pt < 6;  ++pt)
			number[cc] += count[pt][cc];
	}

	/* Open image with new size.  */

	image.Free();

	if (number[0] == 0 && number[1] == 0)
		/* Don't open 0 x 0 image.  */
		return;

	int max_length= 10; /* max number of pieces on one line */ 

	GdkPoint dimension= {std::min(std::max(number[0], number[1]), max_length),
			     std::max((number[0] + max_length - 1) / max_length, 1)  +
			     std::max((number[1] + max_length - 1) / max_length, 1)};
	/* in small square units */ 
	GdkPoint new_size= {width * dimension.x, width*dimension.y};

	if (! image.Create(new_size))
		return;
	if (! image.Has_Canvas())
		return;

	/* draw pieces  */
	GdkColor white;
	gdk_color_white(gdk_colormap_get_system(), &white);
	gdk_gc_set_foreground(image->Get_GC(), &white); 
	image().Draw_Rect((GdkRectangle){0, 0, new_size.x, new_size.y});
	Scaled_Board_Image sbi;
	if (! sbi.Load(width /*, true */ ))
		return;
	
	int line= 0;
	for (int cc= 0;  cc < 2;  ++cc)
	{
		int index= 0;
		int dc= turned ? cc : 1^cc;
		for (int pt= 5;  pt >= 0;  --pt)
		{
			for (int i= 0;  i < count[pt][dc];  ++i)
			{
				if (index == max_length)
				{
					index= 0;
					++ line;
				}
				GdkPoint point= {index*width, line*width};
				sbi.Draw(image(),
					 point,
					 pt,
					 dc);
				++index;
			}
		}
		++ line;
	}
}

void Material_Widget::Image_Widget_Get_Dimension(int &new_width,
						 int &new_height)
{
	new_width= 0;
	new_height= 2 * width;
}

bool Material_Widget::Show(const Position &position)
{
	for (int i= 0;  i < 6;  ++i)
		if (position.Current_Material(0, i) != position.Current_Material(1, i))
			return true;
	return false;
}

void Material_Widget::Set(const Position &new_position,
			  bool new_turned)
{
	position= new_position;
	turned= new_turned; 
	Invalidate_Image();
}

bool Material_Widget::Open(Pointer <Widget> parent,
			   Open_Info)
{
	if (! Image_Widget::Open(parent))  return false;
	
	return true;
}

void Material_Widget::Set_Turned(const Position &position,
			   bool turned)
{
	if (Is() && GTK_WIDGET_VISIBLE(Get()))
		Set(position, turned);
}

void Material_Widget::Update(const Position &position,
			     bool turned,
			     Pointer <Widget> box)
{
	if (Show(position))
	{
		if (! Is())
			Open(box);
		if (Is())
		{
			Set_Visible(true);
			Set(position, turned);
		}
	}
 	else if (Is() && GTK_WIDGET_VISIBLE(Get()))
 	{
 		Set(position, turned);
 	}
}

