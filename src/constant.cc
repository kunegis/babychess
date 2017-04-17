#include "constant.hh"

#if HAVE_MEMORY_LAYOUT

#include <malloc.h>
#include <unistd.h>

void *constant_heap= NULL;

void Constant_Make()
{
	assert (constant_heap == NULL);

	struct mallinfo mi= mallinfo();

	constant_heap= (char*)sbrk(0) - mi.arena;

	assert (constant_heap != NULL);
}

#endif /* HAVE_MEMORY_LAYOUT */
