#include "png.hh"

/* Use LibPNG.  Code adapted from the libpng(3) manpage.  See
 * <http://www.libpng.org/pub/png> for more information. 
 */

#include "verbose.hh"

#include <png.h>

bool PNG::Write(FILE *fp,
		const Image &image,
		bool interlaced)
{
	Verbose_Function();

	assert (image.Is());


	/* Manpage snipplets begin here */
	png_structp png_ptr= png_create_write_struct 
		(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, 
		 NULL, NULL); 
	if (!png_ptr) 
		return (false); 

	png_infop info_ptr = png_create_info_struct(png_ptr); 
	if (!info_ptr) 
	{ 
		png_destroy_write_struct(&png_ptr, 
					 (png_infopp)NULL); 
		return (false); 
	} 

	if (setjmp(png_jmpbuf(png_ptr))) 
	{ 
		png_destroy_write_struct(&png_ptr, &info_ptr); 
		return (false); 
	} 

	png_init_io(png_ptr, fp); 


	/* setting  callback omitted */

	png_set_IHDR(png_ptr, 
		     info_ptr,
		     image.Size().x,
		     image.Size().y, 
		     8,
		     PNG_COLOR_TYPE_RGB,
		     interlaced
		     ? PNG_INTERLACE_ADAM7
		     : PNG_INTERLACE_NONE, 
		     PNG_COMPRESSION_TYPE_DEFAULT, 
		     PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr); 
	
	int size_y= image.Size().y;
	png_byte *row_pointers[size_y];
	for (int i= 0; i < size_y; ++i)
	{
		assert (image.Size().x > 0);
		row_pointers[i]= (png_byte *)malloc(image.Size().x * 3);
		if (row_pointers[i] == NULL)
		{
			for (int j= 0; j < i; ++j)
				free(row_pointers[j]);
			png_destroy_write_struct(&png_ptr, &info_ptr); 
			return false;
		}
		for (int x= 0; x < image.Size().x; ++x)
		{
			int r, g, b;
			image.Get_Pixel(x, i, r, g, b);
			row_pointers[i][x * 3 + 0]= r;
			row_pointers[i][x * 3 + 1]= g;
			row_pointers[i][x * 3 + 2]= b;
		}
	}

	png_write_image(png_ptr, (png_byte **)row_pointers); 

	for (int i= 0; i < size_y; ++i)
		free(row_pointers[i]);


	png_write_end(png_ptr, info_ptr); 

	png_destroy_write_struct(&png_ptr, &info_ptr); 

	return true;
}

bool PNG::Read(FILE *fp,
	  Image            &image,
	  Pointer <Image>   alpha)
{
	Verbose_Function(); 
	
	char buf[8];
	fread(buf, 1, 8, fp); 
	if (png_sig_cmp((png_byte *)buf, 0, 8))
		/* not a PNG file */
		return false;
	
	png_structp png_ptr= png_create_read_struct 
		(PNG_LIBPNG_VER_STRING, 
		 NULL, 
		 NULL, 
		 NULL); 
	if (png_ptr == NULL) 
		return false;

	png_infop info_ptr= png_create_info_struct(png_ptr); 
	if (info_ptr == NULL) 
	{ 
		png_destroy_read_struct(&png_ptr, 
					(png_infopp)NULL,
					(png_infopp)NULL); 
		return false;
	} 

	png_infop end_info= png_create_info_struct(png_ptr); 
	if (end_info == NULL) 
	{ 
		png_destroy_read_struct(&png_ptr, &info_ptr, 
					(png_infopp)NULL); 
		return false; 
	} 

	if (setjmp(png_jmpbuf(png_ptr))) 
		/* LibPNG functions will jump back here on error. 
		 */
	{ 
		png_destroy_read_struct(&png_ptr, &info_ptr, 
					&end_info); 
		return false; 
	} 

	png_init_io(png_ptr, fp); 

	png_set_sig_bytes(png_ptr, 8); 

	png_read_info(png_ptr, info_ptr); 

	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type,
		compression_type, filter_method;

	png_get_IHDR(png_ptr, info_ptr, &width, &height, 
		     &bit_depth, &color_type, &interlace_type, 
		     &compression_type, &filter_method); 

	/* Try to read the data as 32 bit RGBA. 
	 */

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);
 
	if (color_type == PNG_COLOR_TYPE_GRAY &&
	    bit_depth < 8) 
		png_set_gray_1_2_4_to_8(png_ptr);

	bool alpha_image= color_type & PNG_COLOR_MASK_ALPHA;
 
	if (png_get_valid(png_ptr, 
			  info_ptr,
			  PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
		alpha_image= true;
	}

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	if (bit_depth < 8)
	{
		png_set_packing(png_ptr);
	}

	if (bit_depth == 8 && 
	    color_type == PNG_COLOR_TYPE_RGB) 
		png_set_filler(png_ptr,
			       0,
			       PNG_FILLER_AFTER);
	/* If no alpha channel, fill A with a constant. 
	 */

	if (color_type == PNG_COLOR_TYPE_GRAY ||
	    color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	png_byte *rows[height];
	
	for (int i= 0; i < (int)height; ++i)
	{
		rows[i]= (png_byte *)malloc(width * 4);
		if (rows[i] == NULL)
		{
			for (int j= 0; j < i; ++j)
				free(rows[j]);
			png_destroy_read_struct(&png_ptr, 
						&info_ptr, 
						&end_info); 
			return false;
		}
	}

	png_read_rows(png_ptr, 
		      rows,
		      NULL,
		      height);

	png_read_end(png_ptr, end_info);

	png_destroy_read_struct(&png_ptr, 
				&info_ptr,
				&end_info);

	if (! image.Create((GdkPoint){width, height}))
	{
		for (int i= 0; i < (int)height; ++i)
			free(rows[i]);
		return false;
	}

	if (alpha &&
	    ! alpha().Create((GdkPoint){width, height}))
	{
		for (int i= 0; i < (int)height; ++i)
			free(rows[i]);
		return false;
	}

	for (int row= 0; row < (int)height; ++row)
		for (int col= 0; col < (int)width; ++col)
		{
			int mask= 0xFF;
			if (alpha && 
			    alpha_image)
			{
				mask= rows[row][4 * col + 3];
				alpha().Draw_Pixel(col,
						   row,
						   mask,
						   mask,
						   mask);
			}
			image.Draw_Pixel(col,
					 row,
					 rows[row][4 * col + 2]  & mask,
					 rows[row][4 * col + 1]  & mask,
					 rows[row][4 * col + 0]  & mask);
		}

	return true;
}
