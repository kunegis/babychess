#ifndef CONSTANT_HH
#define CONSTANT_HH

/* Dirty module used by STRING, which should be replaced by C++ strings. 
 */

/* Find out whether a pointer refers to constant data.  Always returns
 * FALSE.  Used in STRING. 
 */

#include "../config.hh"

#if HAVE_MEMORY_LAYOUT

#include "inline.hh"

#include <stdlib.h>
#include <assert.h>

extern void *constant_heap;

void Constant_Make();
/* Initialize CONSTANT_HEAP. 
 */

Inline bool Constant(const void *pointer)
/* Determine whether POINTER is not in the heap/stack. 
 */
{
	assert (pointer != NULL);

	if (constant_heap == NULL)
		Constant_Make();

	return pointer < constant_heap;
}

#else /* ! HAVE_MEMORY_LAYOUT */

#include "inline.hh"

Inline bool Constant(const void *)
{
	return false;
}

#endif /* ! HAVE_MEMORY_LAYOUT */

#endif /* ! CONSTANT_HH */
