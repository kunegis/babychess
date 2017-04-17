/* Used to find memory leaks. 
 */

#include "main.hh"
#include "vector.hh"
#include "read_line.hh"

#include <ctype.h>

class Info
{
public:

	String id;
	String pointer;
};

int Main(int, char **)
{
	String line;
	Vector <Info> infos;

	while (Read_Line(stdin, line))
	{
		switch (line[0])
		{
		default:  assert (0);
			
		case 'm':
		{
			if (! infos.Set(*infos+1))
				assert (0);
			Info &info= infos[-1];
			int i= 1;
			while (isdigit(line[i]))  i++;
			info.id= String(line() + 1, i - 1);
			assert (line[i] == ',');
			++i;
			info.pointer= line() + i;
			break;
		}
		case 'r':
		{
			int i= 1;
			while (line[i] != ',' && line[i] != '\0')  ++i;
			assert (line[i] == ',');
			String new_pointer(line() + 1, i - 1);
			String old_pointer= line() + i + 1;
			int size= *infos;
			for (int i= 0;  i < size;  ++i)
			{
				if (infos[i].pointer == old_pointer)
				{
					infos[i].pointer= new_pointer;
					goto end;
				}
			}
			assert (0);
		end:
			break;
		}
		case 'f':
		{
			String pointer= line() + 1;
			int size= *infos;
			for (int i= 0;  i < size;  ++i)
			{
				if (infos[i].pointer == pointer)
				{
					if (i + 1 < *infos)
						infos[i]= infos[-1];
					infos= -1;
					goto end2;
				}
			}
			assert (0);
		end2:
			break;
		}
		}
	}

	for (int i= 0;  i < *infos;  ++i)
	{
		printf("id= %s\n", infos[i].id());
	}

	return 0;
}
