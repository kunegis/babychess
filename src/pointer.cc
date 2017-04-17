#include "pointer.hh"

#ifndef NDEBUG 

#include "verbose.hh"

class Pointer_Info
{
public:
	const void *object;  /* pointer to Type used for
			      *	registration. */ 
	int size; 
	const char *type_name; 
};

/* Note:  the list never freed */
Pointer_Info *pointer_list= NULL;
int           pointer_list_size= 0; 

bool pointer_check= false;

void Pointer_Set(const void *object,
	    int size,
	    const char *name,
	    const Pointed *thisptr)
{
	assert (size >= 0);

	if (thisptr == (void *)0x3)
	{
//		Dbg_("\there");
	}

	assert (object != NULL); 

	assert (pointer_list_size >= 0); 
	Pointer_Info *new_list= (Pointer_Info *)
		( pointer_list_size 
		  ? realloc
		  (pointer_list,
		   (pointer_list_size + 1) * sizeof (Pointer_Info))
		  : malloc((pointer_list_size + 1) * sizeof (Pointer_Info)));
	assert (new_list != NULL); 
	pointer_list= new_list;
	pointer_list[pointer_list_size].object= object;
	pointer_list[pointer_list_size].size= size; 
	pointer_list[pointer_list_size].type_name= name; 
	++ pointer_list_size; 
}

void Pointer_Remove(const void *object, 
		    int size,
		    const char *name)
{
	name= name; 

	assert (size >= 0);

	for (int i= 0;  i < pointer_list_size;  ++i)
	{
		if (pointer_list[i].object == object     &&
		    pointer_list[i].size == size)
		{
			if (i != pointer_list_size - 1)
			{
				pointer_list[i].object= 
					pointer_list[pointer_list_size - 1].object; 
				pointer_list[i].size=
					pointer_list[pointer_list_size - 1].size;
				pointer_list[i].type_name= 
					pointer_list[pointer_list_size - 1].type_name; 
			}
			-- pointer_list_size; 

			return; 
		}
	}
	assert (0);
}

bool Pointer_Hook(void *object, 
		  int size,
		  const char *name)
{
	name= name;

	for (int i= 0;  i < pointer_list_size;  ++i)
	{
		if (object < (void *)((char *)pointer_list[i].object
				      + pointer_list[i].size)		    &&
		    pointer_list[i].object < (void *)((char *)object + size))
		{
			assert (0);
			return false; 
		}
	}
	return true; 
}

static int pointer_halt= 0;

Pointed::~Pointed()
{
	assert (pointer_halt || ref_count == 0); 
}

void AAA()
{
}

#define POINTER_TRACED  0xbfffefc4

void Pointed::Inc(const char *new_name,
		  const void *new_pointer) const
{
	if (pointer_halt)  return;
	if (this == (void*)POINTER_TRACED)
	{
		AAA();
	}
	/*((Pointed *)this)->*/name= new_name; 
	/*((Pointed *)this)->*/pointer= new_pointer; 
	++ /*((Pointed *)this)->*/ref_count;
}

void Pointed::Dec() const
{
	if (pointer_halt)  return;
	if (this == (void*)POINTER_TRACED)
	{
		AAA(); 
	}
	assert (ref_count > 0);
	-- /*((Pointed *)this)->*/ ref_count; 
}

static
class Pointer_Main
{
public:

	Pointer_Main()
		{
			Moveable_Set_Hook(& Pointer_Hook); 
		}

} pointer_main; 

void Pointer_Halt()
{
	pointer_halt= 1;
}

#endif /* ! NDEBUG */ 

