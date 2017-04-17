#ifndef FORMAT_TIME_HH
#define FORMAT_TIME_HH

#include "string.hh"

String Format_Time(int s,
		   int ms_digits= -1,
		   int ms= 0);
/* Return the text representation of SECONDS:MILLISECONDS. 
 *
 *  MS_DIGITS:  number of digits of the milliseconds to show.  -1 ==
 *              automatic. 
 */

#endif /* ! FORMAT_SEC_HH */
