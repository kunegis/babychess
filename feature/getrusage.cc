#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main(int, char **)
{
	clock_t begin= clock();

	int i= 0;
	while (clock() - begin < CLOCKS_PER_SEC)
		++i;

	struct rusage usage;
	getrusage(RUSAGE_SELF, & usage);

	int sec= usage.ru_utime.tv_sec + usage.ru_stime.tv_sec; 
	int usec= usage.ru_utime.tv_usec + usage.ru_stime.tv_usec; 

	printf("time:  %d, %d\n", sec, usec);

	if (sec == 0 && usec == 0)
	{
		printf("getrusage(RUSAGE_SELF) returns zero.\n"); 
		return 1;
	}

	return 0; 
}
