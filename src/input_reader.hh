#ifndef INPUT_READER_HH
#define INPUT_READER_HH

#include "vector.hh"

class Input_Reader; 

/* These must be defined elsewhere.  
 */
extern int Register_Input(Input_Reader &);
extern void Unregister_Input(Input_Reader &); 

class Input_Reader
	:  virtual public Pointed
{
public:

	Input_Reader()
		:  index (-1), fd(-1)
		{ }
	virtual ~Input_Reader()
		{ if (index >= 0)  Close(); }

	/* Called when FILE is ready.  May call Close().  */
	virtual void IR_Input()  { } 

	bool Open(int fd);

	void Close(); 

	int FD() const
		{
			assert (Is());
			return fd; 
		}

	bool Is() const
		{
			assert ((index < 0) == (fd < 0));
			assert (index < 0 || fd == list[index]().fd); 
			return index >= 0; 
		}

	int ID() const
		{ return id;  }

	/* The list may contain NULL entries.  */
	static const Vector <Pointer <Input_Reader> > &Get_List()
		{  return list;  }

private:

	int index; 
	/* in LIST, or -1 when closed. */

	int fd; /* -1 when closed, and >=0 when open. */
	int id; 

	static Vector <Pointer <Input_Reader> > list;
	/* some may be NULL. */
};

#endif /* ! INPUT_READER_HH */
