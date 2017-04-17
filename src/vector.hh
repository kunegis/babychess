#ifndef VECTOR_HH
#define VECTOR_HH

/*    Vector <Type>
 *
 * An array of variable size. 
 *   
 * A vector holds N objects of the same type.  The number of objects
 * can be changed dynamically.  There cannot be vectors of objects
 * that are not moveable, as defined by the MOVEABLE module.
 *
 * The objects in the vector are moved in memory.  Their addresses are
 * not constant. 
 */

#include "object.hh"
#include "moveable.hh"
#include "pointer.hh"
#include "verbose.hh"
//#include "type_name.hh"

class Vector_Base
{
public:
	Vector_Base() :  pointer(NULL), size(0), place(0) {}

protected:
	void *pointer; 
	/* Pointer to array of objects. NULL if SIZE is 0. */
	int size;
	/* Number of objects on array.  If 0, POINTER is NULL.  */
	int place;
	/* The number of objects that fit in *POINTER.  PLACE >= SIZE.    */

	bool Set(int,
		 void (*constructor)(void*, int, int),
		 void (*destructor)(void*, int, int),
#ifndef NDEBUG
		 void (*assert_moveable)(void*, int, int),
#endif
		 int sizeof_type);

	bool Append_Transfer_From(Vector_Base &vector,
				  int i,
#ifndef NDEBUG
				  void (*assert_moveable)(void*, int, int),
#endif
				  int sizeof_type);
};

template <class Type>
class Vector
	:  public Pointed,
           public Vector_Base
{
public:

	Vector() { }
	Vector(const Vector &vector)
		:  Pointed(), Vector_Base()
		{ *this= vector;  }

	~Vector() 
		{
			for (int i= 0; i < size; ++i)
				(p()+i)->~Type();
			free (p()); 
		}

	int operator * () const { return size;  }

	bool Is() const { return size != 0; }
	/* Wether size is greater than 0 */


	const Type *operator () () const  { return p(); }
	Type *operator () ()  { return p(); }
	Type *operator () (int i)
		{
			int index= i < 0 ? i + size : i;
			assert (index >= 0 && index <= size);
			return p() + index;
		}

	const Type *operator () (int i) const
		{
			int index= i < 0 ? i + size : i;
			assert (index >= 0 && index < size);
			return p() + index;
		}

	Type &operator [] (int i)
		{
			assert(i >= - size && i < size);
			int index= i < 0 ? i + size : i;
			assert (index >= 0 && index < size);
			return p()[index];
		}

	const Type &operator [] (int i) const
		{
			assert(i >= - size && i < size);
			int index= i < 0 ? i + size : i;
			assert (index >= 0 && index < size);
			return p()[index];
		}

	void Free()  { Object_Reset(Vector <Type>, this);  }

	bool Set(int n)
	/* Change the size.  Return FALSE on error.  Making
	 * smaller always succeeds.  A negative value of -N is
	 * interpreted as shrinking the vector by N elements.  */ 
		{
			return Vector_Base::Set(n,
						& Vector_Constructor,
						& Vector_Destructor,
#ifndef NDEBUG
						& Vector_Assert_Moveable,
#endif
						sizeof (Type)); 
		}

	int operator = (int new_size)
		/* Change the size.  Lose the return value.  Useful
		 * for values <=0. */
		{ Set(new_size);  return new_size;  }

	Type *Detach ()
		{
			Type *p= p();
			p()= NULL;
			size= 0;
			place= 0;
			return p;
		}

	void operator = (const Vector <Type> &vector)
		{
			Free();

			if (! Set(*vector))
				return;
			for (int i= 0;  i < size;  ++i)
				(*this)[i]= vector[i];
		}

	bool Append(const Type &t)
		{
			if (! Set(**this + 1))
				return false;
			(*this)[-1]= t;
			return true;
		}

	void operator += (const Type &object)
		{
			Append(object); 
		}

	bool Append(const Vector <Type> &vector)
		{
			int size= **this;
			if (! Set(**this + *vector))
				return false;
			for (int i= 0;  i < *vector;  ++i)
				(*this)[size + i]= vector[i]; 
			return true;
		}
	bool Append_Transfer_From(Vector <Type> &vector,
				  int i= 0)
	/* Append objects from VECTOR[i] to the end of VECTOR.  Shrink
	 * VECTOR to the size I.  Return FALSE on error.   On error,
	 * nothing changes.  */
		{
			return Vector_Base::Append_Transfer_From
				(vector, 
				 i,
#ifndef NDEBUG
				 & Vector_Assert_Moveable,
#endif
				 sizeof (Type));
		}

	static void Vector_Constructor(void *po,
				  int a, int b)
		{
			for (int i= a; i < b; ++i)
				new (((Type *)po)+i) Type();
		}
	static void Vector_Destructor(void *po,
				  int a, int b)
		{
			for (int i= a; i < b; ++i)
				(((Type *)po)+i)->~Type();
		}
#ifndef NDEBUG
	static void Vector_Assert_Moveable(void *po,
				  int a, int b)
		{
			for (int i= a; i < b; ++i)
				assert(Moveable(((Type *)po) + i)); 
		}
#endif 

private:

	Type *&p()
		{ return (Type *&)Vector_Base::pointer; }
	Type *p() const
		{ return (Type *)Vector_Base::pointer;  }

};

#endif /* ! VECTOR_HH */
