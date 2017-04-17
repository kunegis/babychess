#ifndef STREAM_FILE_HH
#define STREAM_FILE_HH

/* Read and write an object from/to a file.  
 * Return FALSE on error.  
 */

#include "string.hh"
#include "inline.hh"

#include <stdio.h>

/* Default implementations for this module is compiled separately. 
 */
Inline bool Read_Stream(FILE *, void *) {return false;}
Inline bool Write_Stream(FILE *, const void *) {return false; }
Inline bool Check_Filename(String, void *) {return false; }

bool Read_From_File_Helper(const char *filename,
			   void *object,
			   bool (*) (FILE *, void *));

template <class Type> 
bool Read_From_File(const char *filename,
		    Type &object)
{
	bool Read_Stream(FILE *, Type *);

	typedef bool (*exact_type) (FILE *, Type *);
	
	exact_type p= &Read_Stream; 
	return Read_From_File_Helper
		(filename,
		 &object,
		 (bool (*) (FILE *, void *)) p);
}

bool Write_To_File_Helper(const char *filename,
			  const void *object,
			  bool (*) (FILE *, const void *));

template <class Type>
bool Write_To_File(const char *filename,
		   const Type &object)
{
	bool Write_Stream(FILE *, const Type *);
	typedef bool (*exact_type) (FILE *, const Type *);
	exact_type p= &Write_Stream;
	return Write_To_File_Helper
		(filename, &object, (bool (*) (FILE *, const void *)) p);
}

String Write_To_Tmp_File_Helper
(const void *object, 
 bool (*) (FILE *, const void *));

/* Write to a tmp file and return name of tmp file.  Return "" on error. 
 */
template <class Type>
String Write_To_Tmp_File(const Type &object)
{
	bool Write_Stream(FILE *, const Type *);
	typedef bool (*exact_type) (FILE *, const Type *);
	exact_type p= &Write_Stream;
	return Write_To_Tmp_File_Helper
		(&object, (bool (*) (FILE *, const void *)) p);
}

#endif /* ! STREAM_FILE_HH */
