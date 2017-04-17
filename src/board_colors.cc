#include "board_colors.hh"

#include "install_dir.hh"
#include "filename.hh"
#include "stream_file.hh"
#include "cassert.hh"
#include "magic.hh"

void Board_Colors::Reset()
{
	String filename= Install_Dir() / "share/board_colors/" /
		Default_Conf_Name();

	if (Read_From_File(filename(), *this))
		return;

	/* Some standard values, but the init file should really be
	 * loaded  */


	flags= 0; 

	cassert (c_count == 8);

	colors[0]= RGB(0xff, 0xff, 0xff);		
	colors[1]= RGB(0xe2, 0xe2, 0xe2);
	colors[2]= RGB(0xfc, 0xfe, 0x73); 
	colors[3]= RGB(0xaa, 0xaa, 0xaa);
	colors[4]= RGB(0x99, 0x99, 0xff);
	colors[5]= RGB(0x00, 0x00, 0x00); 
	colors[6]= RGB(0x00, 0x00, 0x00);
	colors[7]= RGB(0xFF, 0x00, 0x00);
}

bool Read_Stream(FILE *file, 
		 Board_Colors *settings)
{
	if (! Magic::Read(file, "Board_Theme"))  return false;

	if (1 != fscanf(file, " %d\n", & settings->flags))
		return false;
	for (int i= 0;  i < c_count;  ++i)
		if (3 != fscanf(file,
				" %d %d %d\n",
				& settings->colors[i].r,
				& settings->colors[i].g,
				& settings->colors[i].b))
		{
			if ( i < 2 )  return false; 
			Board_Colors def;
			def.Reset();
			for (int j= i;  j < c_count;  ++j)
				settings->colors[j]= def.colors[j]; 
		}
	
	return true; 
}

bool Write_Stream(FILE *file, 
		  const Board_Colors *settings)
{
#if 0
	/* Don't save turnedness. */
#endif /* 0 */ 

	if (! Magic::Write(file, "Board_Theme"))  return false;

	if (0 > fprintf(file,
			"%d\n",
			settings->flags /*& (~ bt_turned)*/))
		return false;

	for (int i= 0;  i < c_count;  ++i)
	{
		if (0 > fprintf(file,
				"%d %d %d\n",
				settings->colors[i].r,
				settings->colors[i].g,
				settings->colors[i].b))
			return false;
	}

	return true;
}
