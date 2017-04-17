#include <stdlib.h>
#include <malloc.h>

int a;

int main()
{
	int b;
	void *c= malloc(0x100);
	char *d;
	struct mallinfo mi;

	mi= mallinfo();

	if (c == NULL)
		return 1;

	d= "hello";

	return ((c > &a  &&
	         c < &b  &&
		 c > d)
		? 0 : 1);
}
