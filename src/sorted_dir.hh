#ifndef SORTED_DIR_HH
#define SORTED_DIR_HH

/* Read directories and return the sorted list.  Interface similar to
 * DIR. 
 */

#include <stdlib.h>

class Sorted_Dir
{
public:
	Sorted_Dir()
		:  list(NULL)
		{ }
	Sorted_Dir(const char *name, bool translate= false)
		:  list(NULL)
		{
			Open(name, translate);
		}
	~Sorted_Dir()
		{
			if (Is())  Close();
		}
	bool Is() const {return list != NULL; }
	bool Open(const char *dir_name, bool translate= false);
	void Close();
	const char *Next();

private:

	struct dirent **list;
	int count, index; /* when list != NULL */ 
};

#endif /* ! SORTED_DIR_HH */ 
