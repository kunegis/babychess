#ifndef RAW_STREAM_HH
#define RAW_STREAM_HH

/* For a type called Type, the following two functions are defined:  
 *
 * bool  Read_File(File &file, Type &object);
 * bool Write_File(File &file, const Type &object);
 *
 * On error, FALSE is returned and ERROR is set. 
 */

#include "inline.hh"

/* The following macro defines the two functions for Type.  The
 * functions just the data from core to the file.  
 */
#define STREAM_DECLARE_RAW(Type)    		\
						\
Inline bool					\
Read_Stream(FILE *file,       		        \
	    Type *object)			\
{						\
	return Read_Stream_Raw(file, object);   \
} 						\
						\
Inline bool					\
Write_Stream(FILE *file,		        \
	     const Type *object)		\
{						\
	return Write_Stream_Raw(file, object);  \
}

/* The next two functions are called by the macro above. 
 */
template <class Type>
Inline bool
Read_Stream_Raw(FILE *file, Type *object)
{
	return file.Read(/*&*/object, sizeof(Type)) == sizeof (Type);
}

template <class Type>
Inline bool
Write_Stream_Raw(FILE *file, const Type *object)
{
	return file.Write(/*&*/object, sizeof(Type));
}

#endif /* ! RAW_STREAM_HH */
