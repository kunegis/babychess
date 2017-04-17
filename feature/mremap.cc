#include <unistd.h>
#include <sys/mman.h>

int main()
{
	void *map= mmap(NULL, 0x8000, PROT_READ|PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0);

	map= mremap(map, 0x8000, 0x10000, MREMAP_MAYMOVE);

	return 0;
}
