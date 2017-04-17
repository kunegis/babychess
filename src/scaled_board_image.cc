#include "scaled_board_image.hh"

#include "board_image.hh"
#include "format.hh"
#include "filename.hh"
#include "stream_file.hh"
#include "pixmap_stream.hh"
#include "image.hh"
#include "dir.hh"
#include "piece.hh"
#include "settings.hh"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

Vector <Dynamic <Scaled_Board_Image::Image_Info> > Scaled_Board_Image::aa_images;

bool Scaled_Board_Image::Load(int new_width)
{
	assert (new_width > 0);

	/* Already loaded */ 
	if (new_width == width)  return true;

	/* Close */ 
	if (width >= 0)  Remove();

	/* Already open in list */ 
	for (int i= 0; i < *Get_Images(); ++i)
	{
		if (Get_Images()[i]().width == new_width)
		{
			++ Get_Images()[i]().ref_count;
			width= new_width;
			index= i;
			return true;
		}
	}

	/* Append new entry to list */ 
	if (! Get_Images().Set(*Get_Images() + 1))    return false;

	if (! Get_Images()[-1].Alloc())
	{
		Get_Images()= -1;
		return false;
	}

	index= *Get_Images() - 1;

	Get_Images()[-1]().ref_count= 1;
	Get_Images()[-1]().width= width= new_width;

	GdkPoint dimension= {6 * new_width, 2 * new_width};
	Pointer <Alpha_Pixmap> image= Get_Images()[-1]().image; 

//	String dir= String(g_get_home_dir()) / "tmp"; 
//	if (access(dir(), F_OK))   mkdir(dir(), 0777);
	String dir= Settings_Tmp();

	/* Open saved scaled images */ 

	String image_filename= dir /
		Format("babychess.scaled_board_image.%d", 
		       width);
	String alpha_image_filename= dir /
		Format("babychess.scaled_board_image.alpha.%d", 
		       width);

	if (Read_From_File(image_filename(), image().Get_Image())   &&
	    Read_From_File(alpha_image_filename(), image().Get_Alpha_Image()))
	{
		Verbose("\tLoaded from file.");
		Verbose("\timage      = (%d x %d)", 
		    image().Get_Image().Get_Size().x,
		    image().Get_Image().Get_Size().y);
		Verbose("\talpha_image= (%d x %d)", 
		    image().Get_Alpha_Image().Get_Size().x,
		    image().Get_Alpha_Image().Get_Size().y);
		if (image->Get_Image().Get_Size().x != dimension.x  ||
		    image->Get_Image().Get_Size().y != dimension.y  ||
		    image->Get_Alpha_Image().Get_Size().x != dimension.x ||
		    image->Get_Alpha_Image().Get_Size().y != dimension.y)
			/* Images have wrong size */
		{
			image->Get_Image().Free();
			image->Get_Alpha_Image().Free();
			goto generate;
		}
		return true;
	}

	
	/* Generate from source */ 
 generate:
	if (! (image().Get_Image().Create(dimension)       && 
	       image().Get_Image().Has_Canvas()            &&
	       image().Get_Alpha_Image().Create(dimension) &&
	       image().Get_Alpha_Image().Has_Canvas()))
	{
		image= NULL; 
		Get_Images()= -1;
		width= -1;
		return false;
	}		

	Pointer <Alpha_Pixmap> board= Board_Image::Get();

	if (! (board().Is()                        &&
	       board().Get_Image().Has_Canvas()    &&
	       board().Get_Alpha_Image().Has_Canvas()))
	{
		image= NULL; 
		Get_Images()= -1;
		width= -1; 
		return false;
	}

	GdkRectangle rect= {0, 0, board->Get_Image().Get_Size().x,
			    board->Get_Image().Get_Size().y};

	image->Get_Image()().Draw_Stretch_Canvas
		(board().Get_Image()(),
		 (GdkRectangle){0, 0, dimension.x, dimension.y},
		 rect);

	image->Get_Alpha_Image()().Draw_Stretch_Canvas
		(board().Get_Alpha_Image()(),
		 (GdkRectangle){0, 0, dimension.x, dimension.y},
		 rect); 

	/* Save scaled image to file */ 
	Write_To_File(image_filename(), image().Get_Image());
	Write_To_File(alpha_image_filename(), image().Get_Alpha_Image());
	chmod(image_filename(), 0644);
	chmod(alpha_image_filename(), 0644);

	return true;
}

void Scaled_Board_Image::Remove()
{
	assert (width > 0);

	assert (Get_Images()[index]().width == width);
	assert (Get_Images()[index]().ref_count >= 1);

	-- Get_Images()[index]().ref_count;
	/* Keep unused sized.  */
	width= -1;
}

void Scaled_Board_Image::Draw(Canvas &canvas,
			      GdkPoint origin,
			      int pt,
			      int cc)
{
	assert (pt >= 0 && pt < pi_count);
	assert (cc == cc_w || cc == cc_b);
	
	if (! (Get_Image().Has_Canvas()   &&
	       Get_Alpha_Image().Has_Canvas()))
		return;

	canvas.Set_Function(GDK_AND_INVERT);
	canvas.Draw_Canvas(Get_Alpha_Canvas(),
			   (GdkRectangle){origin.x, origin.y,
						  width, width},
			   (GdkPoint){width * pt,
				 width * cc});
	canvas.Set_Function(GDK_OR);
	canvas.Draw_Canvas(Get_Canvas(),
			   (GdkRectangle){origin.x, origin.y,
						  width,width},
			   (GdkPoint){width * pt,
				 width * cc});
}
