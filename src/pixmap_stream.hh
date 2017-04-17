#ifndef PIXMAP_STREAM_HH
#define PIXMAP_STREAM_HH

#include "pixmap.hh"
#include "image_stream.hh"

bool Write_Stream(FILE *, const Pixmap *);
bool Read_Stream(FILE *, Pixmap *);

#endif /* ! PIXMAP_STREAM_HH */
