#ifndef GETTEXT_HH
#define GETTEXT_HH

/* See po/README for more info */ 

#include <libintl.h>
#include <string.h>

#define _(text)  gettext(text)
#define N_(text)         text

#include "inline.hh"

#include <assert.h>

Inline
const char *I_(const char *text)
{
	text= strchr(gettext(text), ':');

	assert (text != NULL);
	/* This would mean the translation doesn't contain ':' */ 

	if (text != NULL)  ++text;

	return text; 
}

#endif /* ! GETTEXT_HH */ 
