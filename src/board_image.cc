#include "board_image.hh"

/* The file "board.png" is based on the chess font Mérida which is distributed
 * as "freeware". 
 */

#include "stream_file.hh"
#include "alpha_pixmap.hh"
#include "filename.hh"
#include "install_dir.hh"
#include "verbose.hh"

#define BOARD_IMAGE_NAME     "board_image/board.png"

Alpha_Pixmap Board_Image::image;

void Board_Image::Load()
{
	Verbose_Function();
	assert (! image.Is()); 
	String filename= Install_Dir()/"share/"BOARD_IMAGE_NAME; 
	Read_From_File(filename(), image);
}
