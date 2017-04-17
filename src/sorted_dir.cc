#include "sorted_dir.hh"

#include "gettext.hh"

#include <assert.h>
#include <dirent.h>
#include <string.h>

int translatesort(const struct dirent **a, 
		  const struct dirent **b)
{
	return strcoll(_((*a)->d_name),
		       _((*b)->d_name));
}

void Sorted_Dir::Close()
{
	assert (Is());

	for (int i= 0;  i < count;  ++i)
		free(list[i]);
	free(list);
	list= NULL;
}

bool Sorted_Dir::Open(const char *dir_name,
		      bool translate)
{
	assert (!Is());
	count= scandir
		(dir_name, &list, NULL,
		 translate 
		 ? (int (*)(const void *, const void *))translatesort 
		 : alphasort);
	if (count < 0)  
	{
		assert (list == NULL);
		return false;
	}
	index= 0;
	assert (list != NULL);
	return true;
}

const char *Sorted_Dir::Next()
{
	assert (Is());
	if (index == count)  return NULL;
	return list[index++]->d_name;
}
