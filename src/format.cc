#include "format.hh"

#include "verbose.hh"
//#ifdef FORMAT_OLD_IMPLEMENTATION

#include <stdarg.h>
#include <stdio.h>

String Format(const char *format, ...)
{
	assert (format != NULL);

	va_list vl;

	va_start(vl, format);

	String result(80, 0);
	int sn;
	/* Hopefully VL can be used several times here. */
 loop:
	sn= vsnprintf(result.c(), *result+1, format, vl);
	if (sn < 0)  return "";
	if (sn > (int)*result)
	{
		Object_Reset(String, &result, sn, 0);
		goto loop;
	}

	result.Set_Length(sn);

#ifndef NDEBUG
	if (Verbose_Enabled(__FILE__))
	{
		enum {l= 15};
		static int count[l+1];
		int i= sn/10;
		if (l < i)   i= l;
		++count[i];
		for (int i= 0; i < l; ++i)
		{
			fprintf(verbose_file, "%u:%u ", i+1, count[i]);
		}
		fprintf(verbose_file, "%u\n", count[l]);
	}
#endif /* ! NDEBUG */ 

#if 0	
#if GNU_LIBC_IMPLEMENTATION
	Sjkdhjshdjshdsjhd
	String result;
	char *r;
	int size= vasprintf(&r, format, vl);
	{
		void *r2= realloc(r, sizeof (String::Head) + size + 1);
		if (r2 == NULL)
		{
			free (r);
		}
		else
		{
			memmove((char*)r2 + sizeof(String::Head), r2, size + 1);
			String::Head *h= (String::Head *) r2;
			h->guard= 0;
			h->length= size;
			h->ref_count= 1;
			h->place= size;
			Object_Reset(String, &result, r2, 0);
		}
	}
#endif /* GNU */ 
#endif /* 0 */ 
	va_end(vl);

	return result;
}

//#endif /* FORMAT_OLD_IMPLEMENTATION */
