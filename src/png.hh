#ifndef PNG_HH
#define PNG_HH

/* Portable Network Graphics. 
 *
 * See RFC 2083 <ftp://ftp.uu.net/graphics/png/documents/>. 
 */

/* To be replaced by gdk-pixbuf; but I don't know how yet.  The alpha channel
 * must be loaded into a separate image. 
 */

#include "image.hh"

#include <stdio.h>

class PNG
{
public:

	static bool Write(FILE *,
			  const Image &image,
			  bool         interlaced= false);
	/* Write PNG representation of IMAGE into FILE.  Return TRUE
	 * on success. 
	 */

	static bool Read(FILE *,
			 Image           &image,
			 Pointer <Image>  alpha= NULL);
	/* Read from file.  Put alpha channel into ALPHA when not
	 * NULL.  Draw alpha channel as black=transparent and
	 * white=opaque. 
	 */
};

#endif /* ! PNG_HH */
