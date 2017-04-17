#include <stdio.h>

int main()
{
	putc_unlocked('4', stdout);
	putc_unlocked('\n', stdout);
	if (0)
		fread_unlocked((void*)"sd", 2, 1, NULL);
	FILE *dev_null= fopen("/dev/null", "r"); 
	if (dev_null != NULL)
		getc_unlocked(dev_null);

	return 0;
}
