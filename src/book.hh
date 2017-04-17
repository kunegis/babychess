#ifndef BOOK_HH
#define BOOK_HH

/* A memory mapped BabyChess book.  See etc/BOOK. 
 */

#include "game.hh"
#include "inttypes.hh"
#include "cassert.hh"

#include "../config.hh"

enum {bo_position= (1 <<  0),   /* entry types */ 
      bo_move=     (1 <<  1),
      bo_tag=      (1 <<  2),
      bo_string=   (1 <<  3),

      bo_type=     (1 <<  4) - 1,
      /* Leave room for future types */ 

      bo_check=    (1 << 10),   /* POSITION:  whether there is check */ 
};

class Book
{
public:
	typedef int32_t         sig_t;
	typedef uint32_t       usig_t;

#if HAVE_FAST_INTTYPES
	typedef int_fast32_t   sigf_t;
	typedef uint_fast32_t usigf_t;
#else
	typedef sig_t          sigf_t;
	typedef usig_t        usigf_t; 
#endif 

	enum {sig_size= sizeof(sig_t)};

	/* in sync with sizeof (Entry) */ 
	enum {entry_shift= 5,
	      entry_size= 32}; 

	struct Position_Entry
	{
		usig_t last, current, comment; /* pointers */ 
		sig_t score, nag;
		sig_t ep, halfmove_clock;
	};
	struct Move_Entry
	{
		usig_t cnext, cprevious; /*moves */
		usig_t lnext, lprevious; /*moves */
		usig_t move; /* in "small" format */ 
		usig_t before, after; /* positions */ 
	};

	struct Entry
	{
		union
		{
			usig_t flags, magic;
		};
		
		union
		{
			struct 
			{
				union
				{
					usig_t dummy;
					char version;
				};
				usig_t endianness, size, first, starting, tags, firstn;
			} header;
			Position_Entry position; 
			Move_Entry move; 

			struct
			{
				usig_t next, previous;
				usig_t name, value; /* string */ 
				char name_string[3 * sig_size];
			} tag;
			struct 
			{
				usig_t next, previous;
				char text[5 * sig_size];
			} string;
		};
	};

	Book()
		:  entries(NULL) 
		{
			cassert (sizeof (Entry) == entry_size);
			cassert (entry_size == (1 << entry_shift)); 
			/* In sync with entry_shift above */
		}
	~Book() {if(entries) Close();}

	bool Is() const {return entries; }
	bool Open_New (const char *filename);
	bool Open_Read(const char *filename); 
	bool Open_Anonymous();
	bool Merge(const Game &game);
	void Merge(const Variation &variation); 
	/* The variation must begin at the current position. 
	 */
	bool Merge_Into_Game(Game &game);
	void Close();
	void Play(const Move &move);
	void Quit_Variation();
	bool Go_Backward();
	/* return whether the move was the main variation */ 
	void Go_To_Start();
	void Go_Forward();
	bool Go_Forward(const Move &move);
	String Comment(sigf_t position= 0);
	/*If 0, current position */
	const Entry &operator [] (unsigned i) const {return entries[i]; }
	const Position_Entry &operator () () const
		{
			assert (entries[current].flags & bo_position);
			return entries[current].position; 
		}
	void Prepend_Tag(String name, String value);
	void Set_Score(usigf_t position, sigf_t score)
		{
			assert (position < entries[0].header.size);
			assert (entries[position].flags & bo_position);
			entries[position].position.score= score; 
		}
	usigf_t Get_Move(move_t move, bool ignore_detail= false) const;
	/* IGNORE_DETAIL:  ignore EP, CAPT and PROM in MOVE.
	 */
	String Get_String(usigf_t i) const;
	String Tag_Name(usigf_t) const;
	String Tag_Value(usigf_t) const;
	String Tag(const char *name) const;

	const Position &Get_Position() const
		{  return position; }
	const Move_Entry &Get_Move_Entry(usigf_t i) const
		{
			assert (entries[i].flags & bo_move);
			return entries[i].move; 
		}
	const Position_Entry &Get_Position_Entry(usigf_t i) const
		{
			assert (entries[i].flags & bo_position);
			return entries[i].position; 
		}
	void Go_Forward(usigf_t move);
	int Move_Number() const {return move_number; }

private:
	Entry *entries;  /* The book is open when not NULL. */
	int fd; 
	/* Valid and open when the book is open, or <0 when open as an
	 * anonymous book and mremap() is available.  */  

	usigf_t current; /* current position */ 
	Position position;
	int page_size, page_shift;
	/* Set in Open_*() */ 
	int page_entries, page_entries_shift;
	/* Entries per page.  Set in th Open_*() functions.  */
	unsigned pages;
	/* Number of pages used */
	int prot; /* for mmap() */ 
	int move_number;
	/* Number of halfmoves since beginning of game */

	void Reset_Position();
	/* Set POSITION to the starting position of the book */ 

	usigf_t Alloc(int count= 1);
	/* Allocate COUNT entries at the end of the book.  Return the
	 * index of first allocated entry.  
	 */

	usigf_t Alloc_String(String text);
	/* Allocate a string containing TEXT, possibly by allocating
	 * several entries.  Return the first entry of the string, in
	 * which PREVIOUS is not set. */ 

	void Set_Sizes();
	void Clear(usigf_t i)
		{ memset(entries+i, '\0', sizeof(Entry));  }
	void Initialize();
	void Merge_Current_Variation_Into_Game(Game &);
};

#endif /* ! BOOK_HH */
