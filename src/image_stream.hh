#ifndef IMAGE_STREAM_HH
#define IMAGE_STREAM_HH

#include "image.hh"
#include "png.hh"

#include <stdio.h>

Inline bool Write_Stream(FILE *file,  const Image *image)
{
	return PNG::Write(file, *image);
}

Inline bool Read_Stream(FILE *file, Image *image)
{
	return PNG::Read(file, *image);
}

#endif /* ! IMAGE_STREAM_HH */
