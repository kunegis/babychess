#ifndef OBJECT_HH
#define OBJECT_HH

/* Resets an object.   I.e., calls the destructor and a constructor. 
 */

#include <stdlib.h>
#include <new>

#define Object_Reset(Type, pointer, args...) \
( 					     \
	(pointer)->~Type(),		     \
	new (pointer) Type(args)	     \
)

/* I don’t know how to use typeof to avoid the Type argument. 
 */

#endif /* ! OBJECT_HH */
