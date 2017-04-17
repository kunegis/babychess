#ifndef ERROR_HH
#define ERROR_HH

/* Per-process error value.  Set by many functions, including those that return
 * "bool".  If errno is -1, the string represents the error. 
 */

/* Error messages should, just as the LibC messages:
 *   - Begin with a capital letter
 *   - Not be terminated by a period 
 */

#include "string.hh"

class Error
{
public:
	static String Get();
	static void Set(String text, String filename= "");
	static void Set_Filename(String new_filename)
		{  filename= new_filename; }

private:
	/* The error message.  Used when ERRNO is -1.  */
	static String text; 
	/* The file that caused the error */ 
	static String filename; 
};

#endif /* ! ERROR_HH */
