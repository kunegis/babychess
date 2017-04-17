#ifndef STDIO_HH
#define STDIO_HH

#include "inline.hh"
#include "string.hh"

#include "../config.hh"

/* If the functions *_unlocked() exist, we use them.  
 */
#if HAVE_UNLOCKED
#    define STDIO_FUNCTION(name)  name ## _unlocked
#else
#    define STDIO_FUNCTION(name)  name
#endif

#define stdio_putc   STDIO_FUNCTION(putc)
#define stdio_fputs  STDIO_FUNCTION(fputs)
#define stdio_fgets  STDIO_FUNCTION(fgets)
#define stdio_fwrite STDIO_FUNCTION(fwrite)

Inline bool stdio_put_string(const String &s, FILE *file)
{
	int len= *s;
	const char *t= s();
	return 1 == stdio_fwrite(t, len, 1, file); 
}

#endif /* ! STDIO_HH */ 
