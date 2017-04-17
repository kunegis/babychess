#ifndef TIME_HH
#define TIME_HH

/*   s  =  seconds
 *  ms  =  milliseconds
 *  us  =  microseconds
 */

#include "inline.hh"
#include "string.hh"

#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

Inline struct timeval Get_Ressource_Usage()
{
	struct rusage usage;

	getrusage(RUSAGE_SELF, & usage);

	struct timeval result;
	result.tv_sec= (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec
		    + (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1000000); 
	result.tv_usec= (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) % 1000000; 

	return result;
}

Inline struct timeval ms_To_Time(int ms)
{
	struct timeval ret;
    
	ret.tv_sec= ms / 1000;
	ret.tv_usec= ms % 1000 * 1000;

	return ret;
}

#if 0
Inline int Time_To_s(struct timeval time)
{
	return time.tv_sec; 
}
#endif /* 0 */ 

Inline int Time_To_ms(struct timeval time)
{
	return time.tv_sec * 1000 + time.tv_usec / 1000;
}

Inline int Time_To_us(struct timeval time)
{
	return time.tv_sec * 1000000 + time.tv_usec;
}

Inline struct timeval operator - (struct timeval b, struct timeval a)
{
	struct timeval ret;

	ret.tv_sec= b.tv_sec - a.tv_sec;
	ret.tv_usec= b.tv_usec - a.tv_usec;
    
	if (ret.tv_usec + 1000000 < 1000000)
	{
		ret.tv_usec += 1000000;
		-- ret.tv_sec;
	}

	return ret;
}

String Format_Time(struct timeval time, const char *format= "%c");
struct timeval Parse_Time(const char *text,
		const char *format= "%c");
/* Both functions operate on the local time, rather than UTC.  Use
 * format strings of LibC.  */

Inline struct timeval Now()
{
	struct timeval ret;
	if (0 != gettimeofday(& ret, NULL))
	{
		ret.tv_sec= 0;
		ret.tv_usec= 0;
	}
	return ret;
}

Inline struct timeval Make_Date(time_t t)
{
	struct timeval tv;
	tv.tv_sec= t;
	tv.tv_usec= 0;
	return tv;
}

Inline int Date_Diff_ms(struct timeval d1,  struct timeval d2)
/* The number of milliseconds between DATE_1 and DATE_2.  DATE_1 is before
 * DATE_2. */
{
	return ((d2.tv_sec - d1.tv_sec) * 1000 + (d2.tv_usec - d1.tv_usec) / 1000);
}


Inline struct timeval operator + (struct timeval tv, int dd)
{
	tv.tv_sec += dd / 1000;
	tv.tv_usec += dd % 1000 * 1000;
	if (tv.tv_usec > 1000000)
	{
		tv.tv_usec -= 1000000;
		++ tv.tv_sec;
	}
	return tv;
}

Inline void operator += (struct timeval &time, int time_diff)
{
	time= time + time_diff;
}

#endif /* ! TIME_HH */
