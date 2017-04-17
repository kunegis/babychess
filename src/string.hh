#ifndef STRING_HH
#define STRING_HH

/* A custom implementation of the "string" class.  "string" from the
 * C++ libraries should really be used.   */

/* SIGSEGV is raised when memory allocation fails.  Therefore string
 * operations are considered to always succeed.   */

#include "pointer.hh"
#include "constant.hh"
#include "object.hh"

#include <string.h>
#include <assert.h>

class String
	:  public Pointed
{
public:
	class Head
	{
	public:
		int guard;
		/* So that sizeof(Head) aligns.  Put in front.  Always
		 * 0. 
		 */
		int length;
		/* Length of string whithout '\0'.  The string
		 * may contain embedded '\0' characters.
		 */
		int ref_count;
		/* Reference count. 
		 */
		int place;
		/* Maximum length of string that fits into this
		 * whithout resizing. 
		 */

		char string[0];
	};

	String()
		:  raw(NULL)  { }
	String(const String &);
	String(const char *string)
		{
			if (string && !*string)
				string= NULL; 
			constant= string; 
			if (constant && ! Constant(constant))
				Isolate(); 
		}
	String(const char *string, 
	       int length);
	/* Make isolated */

	String(char);

	String(int length, int dummy);
	/* New string of length LENGTH, not initialized.  The
	 * resulting string may contain '\0' characters.   */

	String(void *new_raw)
		:  raw(new_raw)
		{
			if (raw != NULL && ! Const_Str(raw))
			{
				assert (head->ref_count >= 1); 
				++ head->ref_count; 
			}
		}
	String(void *new_raw, int /* dummy */)
		:  raw(new_raw)
		{ }

	~String()
		{  Free(raw);   }

	bool Is() const 
		{  return raw != NULL;   }

	const char *operator () () const 
		/* Return the string as zero-terminated.  Return "" if
		 * the string is empty.  Never return NULL.  
		 */
		{  return Get_String(raw);  }


	char *c()
		/* String must be full and isolated.  Can be written
		 * to until the String is changed or destroyed.  Use
		 * with care. 
		 */
		{
			assert (Is());
			assert (Is_Isolated());
			return head->string;
		}

	size_t operator * () const
		{
			return (raw == NULL   ? 0 :
				Const_Str(raw) ? strlen(constant) :
				head->length);
		}

	int Ref_Count() const {return head->ref_count; }
	int Place() const {return head->place; }

	void Set_Length(int new_length)
		{
			assert (!Const_Str(raw));
			assert (new_length <= head->place);
			head->length= new_length;
			head->string[new_length]= '\0'; 
		}
	
	char operator [] (int n) const
		/* Get the Nth character of the string.  N can range
		 * from -LENGTH to LENGTH included.  When N is LENGTH,
		 * '\0' is returned.  This[0] is always defined.  For
		 * N < 0, characters are counted from the end.
		 * This[-LENGTH] = This[0]. 
		 */
		{
			int length= **this;
			assert(n >= - length && n <= length);

			if (raw == NULL)
				return '\0';
			else 
			{
				if (n < 0)
					n += length;
				if (Const_Str(raw))
					return constant[n];
				else
					return head->string[n];
			}
		}
	char &operator () (int n) 
		/* Cannot be the terminating '\0'. 
		 */
		{
			int length= **this;
			
			assert(Is() && Is_Isolated()); 
			assert(n >= - length && n < length);

			if (n < 0)
				n += length;
			return head->string[n];
		}

	void New(int);
	/* New length and fill with spaces.  Lose data. 
	 */
	void New_New(int);
	/* new size but whithout initialisation */

	bool Is_Isolated() const
		/* The string is isolated when it is writable.  */
		{
			return raw == NULL || (! Const_Str(raw) && head->ref_count == 1);
		}

	void Isolate();
	/* Make the string an allocated HEAD with REF_COUNT == 1.  */

	String Sub(unsigned begin, unsigned length) const;

	const String &operator= (const String &string)
		{
			Object_Reset(String, this, string);
			return *this;
		}
	const String &operator= (const char *string)
		{
			Object_Reset(String, this, string);
			return *this;
		}
	const String &operator= (char c)
		{
			Object_Reset(String, this, c);
			return *this; 
		}

	String operator+(const String &) const;
	String operator+(const char *string) const
		{
			return *this + String(string); 
		}
	String operator+(char c) const
		{
			return *this + String(c);
		}

	String &operator += (const String &);
	String &operator += (const char *);
	String &operator += (char c)
		{
			return *this += String(c);
		}

	bool operator == (const String &) const;
	bool operator == (const char *string) const
		{
			if (string == NULL)
				return raw == NULL;
			if (raw == NULL)
  				return string == NULL || string[0] == '\0'; 
			return 0 == strcmp((*this)(), string); 
		}
	bool operator == (char c) const
		{
			return **this == 1 && (*this)[0] == c; 
		}

	bool operator != (const String &str) const 
		{
			return !operator==(str);
		}
	bool operator != (const char *string) const
		{
			return ! operator == (string);
		}
	bool operator != (char c) const
		{
			return !operator==(c);
		}

	bool Begins_With(String s) const
		{
			return (! s.Is()      ||
				(Is() && 0 == strncmp((*this)(), s(), *s))); 
		}

	bool operator < (const String &string) const
		{
			return strcmp((*this)(), string()) < 0;
		}
	bool operator > (const String &string) const
		{
			return strcmp(string(), (*this)()) < 0; 
		}

	void *Raw()
		{  return raw;   }
	const void *Raw() const
		{  return raw; }

	void *Detach()
		{
			if (raw && ! Const_Str(raw))
				assert (head->ref_count >= 1); 
			void *result= raw;
			raw= NULL;
			return result;
		}
	void Append_To_Isolated(const char *,
				int length);
	void Append_To_Any(const char *text,
			   int length)
		{
			if (**this)
			{
				Isolate();
				Append_To_Isolated(text, length);
			}
			else
			{
				Object_Reset(String, this, text, length);
			}
		}

	operator bool () const
		{  return raw;   }

	void Free()
		{
			Free(raw);
			raw= NULL;
		}


	/* Functions to handle a "void *" as a string.  (i.e. for
	 * putting them in a hash table.)
	 */
	static void Free(void *raw);
	static const char *Get_String(const void *raw);

private:
	union
	{
		void *raw;
		Head *head;
		const char *constant;
	};

	static void Free_Head(Head *head);
	/* Free the string.  Must be non-empty and non-constant.  */

	static bool Const_Str(const void *p)
		{ return ((const char *)p)[0] != '\0';   }
};

String operator + (char c, String s);

Inline String operator + (const char *string_1, 
			  String string_2)
{
	return String(string_1) + string_2;
}

#endif /* ! STRING_HH */
