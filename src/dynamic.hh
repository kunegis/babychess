#ifndef DYNAMIC_HH
#define DYNAMIC_HH

/* Dynamic objects using Data_Alloc() / Data_Free().  May be replaced by
 * auto_ptr.
 */

#include <assert.h>
#include <stdlib.h>

/* Wrapper around operator new() that catches alloc exceptions and returns
 * NULL.  */
#define Data_Alloc(Type, args...)				\
({								\
	Type *data_alloc_object;				\
	try							\
	{							\
		data_alloc_object= new Type(args);		\
		assert (data_alloc_object != NULL); 		\
	}							\
	catch (...)						\
	{							\
		data_alloc_object= NULL;			\
	}							\
	data_alloc_object;					\
})

template <class Type> 
class Dynamic
{
private:
	Type *pointer;

public:
	Dynamic() 
		:  pointer(NULL)   { }

	Dynamic (const Dynamic <Type> &dynamic)
		{
			if (dynamic.Is())
				pointer= Data_Alloc(Type, dynamic());
			else
				pointer= NULL;
		}

	~Dynamic() 
		{ delete pointer;  }

	const Dynamic <Type> &operator = (const Dynamic <Type> &object)
		/* OBJECT should not be of a type derived from Type.  */
		{
			delete pointer;
			if (object.Is())
				pointer= Data_Alloc(Type, object());
			else
				pointer= NULL;
			return *this;
		}

	template <class U>
	void Transfer_To(Dynamic <U> &u)
		{
			Type *p= Detach();
			u.Attach(p);
		}
	template <class U>
	void operator >> (Dynamic <U> &u)
		{  Transfer_To(u);   }

	template <class U>
	void operator << (Dynamic <U> &u)
		{  u.Transfer_To(*this);   }

	bool Is() const 
		{  return pointer != NULL;  }
	Type &operator () () 
		{  return *pointer;  }
	Type const& operator() () const 
		{  return *pointer;  }

	Type *operator* () 
		{  return pointer;  }

	Type const *operator* () const 
		{  return pointer;  }

	template <class U>
	bool Attach(U *u) 
		/* U is a sub-type of Type.  Return whether U was not
		 * NULL.  
		 */
		{
			if (Is())
				Free();
			pointer= u; 
			return u != NULL;
		}

	Type *Detach()
		{
			Type *p= pointer;
			pointer= NULL;
			return p;
		}

	bool Alloc() 
		/* Must not be.  Make new object using the default constructor.
		 * Return value: FALSE on error, else TRUE. 
		 */
		{
			assert (! Is());
			pointer= Data_Alloc(Type);
			return pointer != NULL;
		}

	bool Alloc(const Type &t)
		/* Like Alloc() but call copy constructor. 
		 */
		{
			assert (! Is());
			pointer= Data_Alloc(Type, t);
			return pointer != NULL;
		}

	template <class U>
	bool Alloc(U &u)
		{
			assert (! Is());
			pointer= Data_Alloc(Type, u);
			return pointer != NULL;
		}

	template <class U, class V>
	bool Alloc(const U &u, const V &v)
		{
			assert (! Is());
			pointer= Data_Alloc(Type, u, v);
			return pointer != NULL;
		}
	template <class U, class V, class W>
	bool Alloc(const U &u, const V &v, const W &w)
		{
			assert (! Is());
			pointer= Data_Alloc(Type, u, v, w);
			return pointer != NULL;
		}

	void Free() 
		/* Need not be */
		{
			delete pointer;
			pointer= NULL;
		}

	operator bool () const
		{ return pointer != NULL;  }
	Type *operator -> () const
		{ return pointer;  }
};

#endif /* ! DYNAMIC_HH */
