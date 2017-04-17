#include "string.hh"

/* A string is either one of:
 *
 *   - Empty:      raw == NULL
 *   - Constant:   Const_Str(raw)
 *   - Owned:      ! Const_Str(raw)
 *
 * If the string is constant, the place RAW points to is a string
 * constant that never changes.
 *
 * If the string is owned, HEAD points to a chunk of of memory that
 * can hold HEAD->PLACE characters.  HEADs are shared between several
 * String objects using reference counting. 
 */

/* The FORMAT module makes use of the internal Head structure. 
 */

#include <signal.h>

void String::Free_Head(Head *head)
{
	assert (head != NULL && ! Const_Str(head));
	assert (head->ref_count >= 1);

	if (head->ref_count > 1)
	{
		-- head->ref_count;
	}
	else
	{
		assert (head->ref_count == 1);
		free(head);
	}
}

String::String(const String &string)
	:  Pointed()
{
	if (! string.Is())
	{
		raw= NULL;
		return;
	}

	if (Const_Str(string.raw))
	{
		constant= string.constant;
	}
	else
	{
		head= string.head;
		++ head->ref_count;
	} 
}


void String::Isolate()
{
	if (raw == NULL)
		return;

	if (Const_Str(raw))
	{
		int length= strlen(constant);
		Head *new_head= (Head *) malloc
			(sizeof (Head) + (1 + length) * sizeof (char));
		if (new_head == NULL)
			raise(SIGSEGV);
		new_head->guard= 0;
		new_head->length= length;
		new_head->place= length;
		new_head->ref_count= 1;
		memcpy(new_head->string,
		       constant,
		       (1 + length) * sizeof (char));
		head= new_head;
	}
	else
	{
		if (head->ref_count == 1)
			return;

		Head *new_head= (Head *) malloc
			(sizeof(Head) + (1 + head->length) * sizeof (char));

		if (new_head == NULL)
			raise(SIGSEGV);

		new_head->guard= 0;
		new_head->length= head->length;
		new_head->ref_count= 1;
		new_head->place= head->length;
		memcpy(new_head->string,
		       head->string,
		       (1 + head->length) * sizeof (char));
		-- head->ref_count;
		head= new_head;
	}	
}

String String::operator + (const String &string) 
	const
{
	if (raw == NULL)
		return string;

	if (string.raw == NULL)
		return *this;

	unsigned own_length= **this;
	unsigned length= *string;

	String result(own_length + length, 0);
	
	if (*result != own_length + length)
		return result;

	memcpy(result.c(),
	       (*this)(),
	       own_length);
	memcpy(result.c() + own_length,
	       string(),
	       length);
	return result;
}

void String::Append_To_Isolated(const char *text,
				int length)
{
	assert (length >= 0);
	assert(text);
	assert(text[length] == '\0');
	assert(raw); 

	Isolate();

	int own_length= head->length;

	if (own_length + length <= head->place)
	{
		memcpy(head->string + own_length,
		       text,
		       1 + length);
		head->length= own_length + length;
	}
	else
	{
		int new_place= head->place * 21 / 20;
		int o= own_length + length;
		if (o > new_place)  
			new_place= o;
		Head *new_head= (Head *)realloc
			(head,
			 sizeof (Head) + new_place + 1);
		if (new_head == NULL)
			raise(SIGSEGV);
		head= new_head;
		head->length += length;
		head->place= new_place;
		memcpy(head->string + own_length,
		       text,
		       length + 1);
	}
}

String &String::operator += (const char *string)
{
	if (!raw)
	{
		*this= string;
	}
	else
		Append_To_Isolated(string, strlen(string)); 

	return *this;
}

bool String::operator == (const String &string) const
{
	return (raw == NULL
		? string.raw == NULL
		: (string.raw == NULL
		   ? false
		   : 0 == strcmp(Const_Str(raw) ? constant : head->string, 
				 Const_Str(string.raw)
				 ? string.constant 
				 : string.head->string))); 
}

String::String(int size, 
	       int /* dummy */)
{
	head= (Head *) malloc(sizeof (Head) + (size + 1) * sizeof (char));
	if (head == NULL)
	{
		raise(SIGSEGV);
		return;
	}
	head->guard= 0;
	head->length= head->place= size;
	head->ref_count= 1;
	head->string[size]= '\0';
}

String::String(const char *string, 
	       int length)
{
	if (length == 0)
	{
		raw= NULL;
		return;
	}

	assert (length > 0);
	assert (string);

	head= (Head *) malloc(sizeof (Head) + length + 1);
	if (head == NULL)
	{
		raise(SIGSEGV);
		return;
	}
	head->guard= 0;
	head->length= head->place= length;
	head->ref_count= 1;
	memcpy(head->string, string, length);
	head->string[length]= '\0'; 
	assert(head->string[length] == '\0'); 
}

String::String(char c)
{
	head= (Head *) malloc(sizeof (Head) + 2 * sizeof (char));
	if (head == NULL)
	{
		raise(SIGSEGV);
		return;
	}
	head->guard= 0;
	head->length= head->place= 1;
	head->ref_count= 1;
	head->string[0]= c;
	head->string[1]= '\0';
}

String &String::operator += (const String &string)
{
	if (string.Is())
	{
		if (!raw)
			*this= string;
		else
		{
			Append_To_Isolated(string(), *string); 
		}
	}

	return *this;
}

String String::Sub(unsigned begin, unsigned length) 
	const
{
	assert (begin + length <= **this);
	
	if (length == 0)
		return "";

	String result(length, 0);

	memcpy(result.c(),
	       (*this)() + begin,
	       length);

	return result;
}

void String::Free(void *raw)
{
	if (raw == NULL)
		return;
	if (Const_Str(raw))
		return;

	Free_Head((Head *) raw);
}

const char *String::Get_String(const void *raw)
{
	return (raw == NULL     ? ""                 :
		Const_Str(raw)   ? (const char *)raw  :
		((const Head *)raw)->string);
}
