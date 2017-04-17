#include "input_reader.hh"

Vector <Pointer <Input_Reader> > Input_Reader::list;

void Input_Reader::Close()
{
	if (index < 0)  return;

	Unregister_Input(*this); 
	list[index]= NULL; 
	index= -1;
	fd= -1;

	int new_size= *list;
	while (new_size != 0 && !list[new_size - 1])
		-- new_size;
	list= new_size; 
}

bool Input_Reader::Open(int new_fd)
{
	assert (new_fd >= 0);
	assert (! Is()); 

	/* Find NULL entry */ 
	for (int i= 0;  i < *list;  ++i)
	{
		if (list[i] == NULL)
		{
			index= i;
			break;
		}
	}

	/* Append */ 
	if (index < 0)
	{
		int size= *list;
		if (! list.Set(size + 1))
			return false;
		index= size;
	}
	list[index]= this;

	fd= new_fd; 

	id= Register_Input(*this); 
  
	return true;
}
