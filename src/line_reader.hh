#ifndef LINE_READER_HH
#define LINE_READER_HH

#include "../config.hh"

#if HAVE_GETLINE

#include <stdio.h>
#include <unistd.h>

class Line_Reader
{
public:
	Line_Reader(FILE *new_file)
		:  file (new_file),
		   n (0),
		   lineptr(NULL)
		{
		}

	bool Read(bool dont_remove_newline= false)
		{
		        size= getline(&lineptr, &n, file);
			if (size < 0)  return false;
			if (!dont_remove_newline)
			{
				lineptr[--size]= '\0';
			}
			return true;
		}
	const char *Line() const
		{
			return lineptr;
		}
	int Length() const {return size; }

private:
	FILE *file;
	size_t n;
	ssize_t size;
	char *lineptr;
};

#else /* ! HAVE_GETLINE */ 

#include "string.hh"

#include <stdio.h>

class Line_Reader
{
public:
	Line_Reader(FILE *new_file)
		:  file(new_file)
		{}

	bool Read (bool dont_remove_newline= false);
	const char *Line() const 
		{
			return line();
		}
	int Length() const {return *line; }

private:
	FILE *file;
	String line;
};

#endif /* ! HAVE_GETLINE */ 

#endif /* ! LINE_READER_HH */ 
