#ifndef DIR_HH
#define DIR_HH

/* Read a directory listing */ 

#include "string.hh"

#include <dirent.h> 

class Dir
{
public: Dir() :  dir (NULL)  { }
	Dir(String directory_name) 
	/* Open the directory DIRECTORY_NAME.  May fail.  Check Is().   */
	/* The directory is closed on destruction.  */
		:  dir (NULL)
		{  Open(directory_name);   }
	bool Is() const  { return dir != NULL; } 
	operator bool () const { return Is();  }
	void Close()	
		/* The directory must be open  */
		{
			assert(Is());  
			closedir(dir);  
			dir= NULL; 
		} 
	~Dir()
		{ if (Is())  Close();  }

	bool Open(String directory_name);

	String Next()
		/* Return "" when closed.  Return "" when end of directory.  Else
		 * return the filename. 
		 */
		{
			String name;
			if (Read(name))  return name;
			else             return ""; 
		}

private:
	DIR *dir;
	/* NULL when closed, else open.  */

	bool Read(String &);
	/* Return FALSE when not open.  
	 * Read the next filename from the directory into FILENAME.
	 * Return TRUE if the next filename was read, and FALSE if
	 * there is no next file.  
	 */

	Dir(const Dir &) 
		{ assert (0);  }
};

#endif /* ! DIR_HH */
