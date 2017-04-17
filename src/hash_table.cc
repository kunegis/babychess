#include "hash_table.hh"

#include <signal.h>

bool Hash_Table::Get(String key, String &value) const
{
	assert (*keys == *values);

	for (int i= 0; i < *keys;  ++i)
		if (keys[i] == key)
		{
			value= values[i];
			return true;
		}
	return false;
}

void Hash_Table::Set(String key,
		     String value)
{
	assert (*keys == *values);
	int size= *keys;
	for (int i= 0; i < size; ++i)
	{
		if (keys[i] == key)
		{
			values[i]= value;
			return;
		}
	}
	if (! (keys.Set(size+1) && values.Set(size+1)))
	{
		raise(SIGSEGV);
		return;
	}
	
	keys[size]= key;
	values[size]= value; 
}

void Hash_Table::Remove(String key)
{
	assert (*keys == *values);
	for (int i= 0; i < *keys;  ++i)
	{
		if (keys[i] == key)
		{
			if (i+1 < *keys)
			{
				keys[i]= keys[-1];
				values[i]= values[-1];
			}
			keys= -1;
			values= -1;
			return;
		}
	}
}

