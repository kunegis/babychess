#include "book.hh"

#include "error.hh"
#include "gettext.hh"
#include "format.hh"
#include "fen.hh"
#include "strnlen.hh"
#include "tmp_dir.hh"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

enum {book_endian_check= 0x04030201};

#define book_magic_4  "bch\x84"
/* 64 bit version would be \x88 */ 

bool Book::Open_New(const char *filename)
{
	assert (entries == NULL);

	fd= open(filename, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (fd < 0)  return false;
	if (0 > ftruncate(fd, 2 * sizeof (Entry)))
	{
		close(fd);
		return false;
	}
	Set_Sizes();
	prot= PROT_READ|PROT_WRITE;
	entries= (Entry*)mmap(NULL, page_size, prot, MAP_SHARED, fd, 0);
	if ((int)entries == -1)  
	{
		entries= NULL; 
		close(fd);
		return false;
	}
	Initialize();

	return true;
}

void Book::Initialize()
{
	pages= 1;

	Clear(0);
	memcpy(&entries[0].magic, book_magic_4, 4);
	entries[0].header.dummy= 0;
	entries[0].header.version= 0;
	entries[0].header.endianness= book_endian_check;
	entries[0].header.size= 2;
	entries[0].header.first= 1;
	entries[0].header.firstn= 1;

	Clear(1);
	entries[1].flags= bo_position;
	
	current= 1;
	move_number= 0;

	position.Reset(); 
}

bool Book::Open_Anonymous()
{
	assert (!entries); 

	Set_Sizes();

	prot= PROT_READ | PROT_WRITE;

#if HAVE_MREMAP

	entries= (Entry*)mmap(NULL, 
			      page_size, 
			      prot,
			      MAP_PRIVATE|MAP_ANONYMOUS, 
			      -1, 0);
	fd= -1;

#else /* ! HAVE_MREMAP */

	String name= Format("%s/%d.XXXXXX", Tmp_Dir(), (int)getpid());
	name.Isolate();
	fd= mkstemp(name.c());
	if (fd < 0)  return false;
	entries= (Entry *)mmap(NULL, page_size, prot, MAP_SHARED|MAP_PRIVATE, fd, 0);

#endif /* ! HAVE_MREMAP */

	if ((int)entries == -1)  
	{
#if ! HAVE_MREMAP
		close(fd); 
#endif /* ! HAVE_MREMAP */ 

		entries= NULL; 
		return false;
	}

	Initialize();
	return true; 
}

bool Book::Merge(const Game &game)
{
	/* Starting position */
	if (game.Starting_Position().Is())
	{
		if (entries[0].header.starting)
		{
			/* assert they are the same */ 
			if (Get_String(entries[0].header.starting) !=
			    FEN(game.Starting_Position()()))
			{
				Error::Set(_("Different starting position"));
				return false;
			}
		}
		else
		{
			if (entries[entries[0].header.first].position.current)
			{
				/* can't create new position:  already
				 * moves. */ 
				Error::Set(_("Different starting position"));
				return false;
			}
			else
			{
				/* set new starting */ 
				String fen= FEN(game.Starting_Position()()); 
				entries[entries[0].header.starting= Alloc_String(fen())]
					.string.previous= 0;
				Reset_Position(); 
			}
		}
	}
	else
	{
		if (entries[0].header.starting)
		{
			/* different starting position */ 
			Error::Set(_("Different starting position"));
			return false;
		}
		else
		{
			/* ok */ 
		}
	}

	/* tags */ 
	String date_text= game.info.Format_Date();
	Prepend_Tag("Event", game.info.event.Is() 
		    ? game.info.event : "?");
	Prepend_Tag("Site",  game.info.site .Is()
		    ? game.info.site  : "?");
	Prepend_Tag("Date",  date_text);
	Prepend_Tag("Round", game.info.round.Is() 
		    ? game.info.round : "?");
	Prepend_Tag("White", game.info.players[cc_w].Is() 
		    ? game.info.players[cc_w] : "?");
	Prepend_Tag("Black", game.info.players[cc_b].Is() 
		    ? game.info.players[cc_b] : "?");
	Prepend_Tag("Result", Game_Info::Get_Result_Text
		    (game.info.result));

	for (int i= 0; i < *game.info.tags; ++i)
	{
		Prepend_Tag(game.info.tags.Key(i), game.info.tags.Value(i));
	}
	
	/* moves */ 
	if (*game.Main_Variation() != 0)
		Merge(game.Main_Variation()); 

	return true; 
}

void Book::Close()
{
	assert (entries);

	munmap(entries, entries[0].header.size << entry_shift);

	if (HAVE_MREMAP && fd >= 0)  
		close(fd); 

	entries= NULL;
}

void Book::Merge(const Variation &variation)
{
	Verbose_Function(); 
	assert (*variation >= 0);
	String first_comment= variation.Get_Comment(0);
	if (first_comment)
	{
		entries[entries[current].position.comment=
			Alloc_String(first_comment())].string.previous= current;
	}
	for (int i= 0;  i < *variation;  ++i)
	{
		Play(variation[i]);
		if (variation.Have_Variations(i))
		{
			Go_Backward();
			for (int j= 0;  j < *variation.Get_Variations(i);  ++j)
			{
				Merge(variation.Get_Variations(i)[j]());
			}
			Go_Forward();
		}

		/* Comment */ 
		String comment= variation.Get_Comment(i+1);
//		if (!i)  comment= variation.Get_Comment(0)+comment;
		if (comment) 
		{
			usigf_t s= Alloc_String(comment());
			entries[entries[current].position.comment= s]
				.string.previous= current;
		}

		/* NAG */ 
		entries[current].position.nag= variation.Get_NAG(i);
	}
	Quit_Variation();
};

void Book::Play(const Move &play_move)
{
	assert (entries[current].flags & bo_position);

	int new_move= Alloc(2);
	Clear(new_move); 
	if (entries[current].position.current == 0)
	{
		entries[current].position.current= new_move;
		entries[new_move].move.cprevious= current;
	}
	else
	{
		int move= entries[current].position.current;
		while (entries[move].move.cnext)
			move= entries[move].move.cnext;
		entries[move].move.cnext= new_move;
		entries[new_move].move.cprevious= move;
	}

	int new_pos= new_move+1;

	entries[new_move].flags= bo_move;
	entries[new_move].move.lprevious= new_pos;
	entries[new_move].move.move= play_move.Small();
	entries[new_move].move.before= current;
	entries[new_move].move.after= new_pos;

	Clear(new_pos); 
	entries[new_pos].flags= bo_position;
	entries[new_pos].position.last= new_move;

	current= new_pos;
	position << play_move; 
	++ move_number;

	entries[new_pos].position.ep= position.EP_Poss();
	entries[new_pos].position.halfmove_clock= position.Halfmove_Clock(); 
	if (position.In_Check()) entries[new_pos].flags |= bo_check; 
}

void Book::Reset_Position()
{
	if (entries[0].header.starting)
	{
		bool parsed= Parse_FEN(Get_String(entries[0].header.starting),
				       position);
		parsed= parsed; 
		assert (parsed);
		/* If the position is stored as a starting position, it must be
		 * a valid FEN string. */
		position.Reset(); 
	}
	else
	{
		position.Reset();
	}

#if 0
	if ((!entries[0].header.starting) || 
	    ! Parse_FEN(Get_String(entries[0].header.starting), position))
		position.Reset();
#endif /* 0 */ 
}

void Book::Quit_Variation()
{
	while (Go_Backward() && entries[current].position.last);
}

bool Book::Go_Backward()
{
	usigf_t move= entries[current].position.last;
	current= entries[move].move.before;
	Move m(entries[move].move.move, 
	       entries[current].position.ep,
	       entries[current].position.halfmove_clock,
	       position[(entries[move].move.move >> 0x8) & 0xFF],
	       entries[current].flags & bo_check);
	position >> m;
	assert (move_number > 0);
	-- move_number;
	return entries[current].position.current == move;
}

void Book::Go_Forward()
{
	usigf_t move= (*this)().current;
	assert (move);
	Go_Forward(move); 
}

Book::usigf_t Book::Alloc(int count)
{
	Verbose("Book::Alloc(count = %d)", count);
	Verbose_Int(fd);
	Verbose_Int(HAVE_MREMAP);

	/* this ignores errors */

	usigf_t size= entries[0].header.size;
	usigf_t new_size= size + count;

	bool need_new_page= new_size > (pages << page_entries_shift);
	
	Verbose_Int(need_new_page);

	bool need_truncate= fd >= 0;
	/* We don't need to truncate the file if we're going to remap it. 	 */

	Verbose_Int(need_truncate); 

	if (need_truncate) 
		ftruncate (fd, new_size << entry_shift);

	if (need_new_page)
	{
		unsigned new_pages= ((new_size - 1) >> page_entries_shift) + 1; 

#if HAVE_MREMAP

		entries= (Entry*)mremap
			(entries,
			 pages << page_shift,
			 new_pages << page_shift,
			 MREMAP_MAYMOVE);

#else /* ! HAVE_MREMAP */
		
		Verbose("noremap A");

		munmap(entries, pages << page_shift);
		Verbose("noremap B");
		entries= (Entry *)mmap(NULL,
				       new_pages << page_shift,
				       prot,
				       MAP_SHARED,
				       fd,
				       0);
		Verbose("noremap C");

#endif /* ! HAVE_MREMAP */ 

		pages= new_pages;
	}

	entries[0].header.size= new_size; 
	return size; 
}

bool Book::Open_Read(const char *filename)
{
	if (entries != NULL)
	{
		Error::Set(_("Book already open"));
		return false;
	}

	Set_Sizes();
	fd= open(filename, O_RDONLY);
	if (fd < 0)  return false;

	/* check header */ 
	Entry header;
	if ((int)sizeof(Entry) > read(fd, &header, sizeof(Entry)))
		goto close;

	if (memcmp(&header, book_magic_4, 4))
	{
		Error::Set(_("Data is not a book of the correct size"));
		goto close;
	}

	if (header.header.version != 0)
	{
		Error::Set(Format(_("Version %d is not supported"), header.header.version));
		goto close;
	}

	if (header.header.endianness != book_endian_check)
	{
		Error::Set(_("Book has incompatible endianness"));
		goto close; 
	}

	struct stat stat;
	if (0 > fstat(fd, &stat))  goto close; 

	if ((unsigned)stat.st_size != (header.header.size << entry_shift)) 
	{
		Error::Set(_("Book has wrong size")); 
		Verbose_Int((int) stat.st_size);
		Verbose_Int(header.header.size << entry_shift);
		goto close;
	}


	/* map the rest */ 
	pages= (header.header.size+page_entries-1)>>page_entries_shift;

	prot= PROT_READ;
	entries= (Entry*)mmap(NULL, pages << page_shift, prot, MAP_SHARED, fd, 0);

	if ((int)entries == -1)  
	{

		entries= NULL; 
		goto close;
	}

	current= header.header.starting;
	move_number= 0;
	return true;

 close:
	close(fd);

	return false;
}

void Book::Set_Sizes()
{
	page_size= sysconf(_SC_PAGESIZE);
	Verbose("page_size = %d", page_size);
	assert ((page_size & (page_size-1)) == 0);
	assert (page_size > (int)sizeof(Entry)); 
	page_entries= page_size >> entry_shift;
	page_entries_shift= -1;
	int s= page_entries;
	while (s)  
	{
		s >>= 1;
		++ page_entries_shift; 
	}
	page_shift= entry_shift + page_entries_shift; 
}

Book::usigf_t Book::Alloc_String(String text)
{
	int count= (*text+(5*sig_size-1))/(5*sig_size);
	usigf_t ret= Alloc(count);
	for (int i= count-2;  i >= 1;  --i)
	{
		entries[ret+i].flags= bo_string;
		entries[ret+i].string.previous= ret+i-1;
		entries[ret+i].string.next=     ret+i+1;
		memcpy(entries[ret+i].string.text,
		       text()+(5*sig_size)*i,
		       5*sig_size);
	}
	if (count > 1)
	{
		entries[ret].flags= bo_string;
		entries[ret].string.next= ret+1;
		memcpy(entries[ret].string.text,
		       text(),
		       5*sig_size);
		entries[ret+count-1].string.previous= ret+count-2;
	}
	entries[ret+count-1].flags= bo_string;
	entries[ret+count-1].string.next= 0;
	int size= ((*text+(5*sig_size-1)) % (5*sig_size) + 2);
	if (size > 5*sig_size)   size= 5*sig_size; 
	memcpy(entries[ret+count-1].string.text,
	       text()+(count-1)*(5*sig_size),
	       size);
	return ret; 
}

void Book::Go_To_Start()
{
	Reset_Position();
	current= entries[0].header.first; 
	move_number= 0;
}

bool Book::Go_Forward(const Move &move)
{
	usigf_t next= entries[current].position.current;
	move_t small= move.Small();
	while (next)
	{
		if (entries[next].move.move == small)
		{
			Go_Forward(next);
			return true;
		}
		next= entries[next].move.cnext; 
	}
	return false;
}

void Book::Go_Forward(usigf_t move)
{
	assert (move);

	/* Check that the move is a current possible move */ 
#ifndef NDEBUG
	usigf_t next= (*this)().current;
	while (next)
	{
		if (next == move)  goto cont;
		next= Get_Move_Entry(next).cnext;
	}
	assert (0);
 cont:
#endif

	Move m(entries[move].move.move, 
	       entries[current].position.ep,
	       entries[current].position.halfmove_clock,
	       position[entries[move].move.move & 0xFF],
	       entries[current].flags & bo_check);
	position << m;
	current= entries[move].move.after;
	++move_number;
}

String Book::Comment(sigf_t position)
{
	if (! position)  position=  current;
	String ret;
	usigf_t next= entries[current].position.comment;
	while (next)
	{
	  int len= strnlen(entries[next].string.text, 5*sig_size);
		String chunk(entries[next].string.text, len);
		ret += chunk;
		next= entries[next].string.next;
	}
	return ret; 
}

void Book::Prepend_Tag(String name, String value)
{
	usigf_t tag= Alloc();
	Clear(tag);
	entries[tag].flags= bo_tag;
	if (entries[0].header.tags)
	{
	    usigf_t next= entries[0].header.tags;
		while (entries[next].tag.next)
			next= entries[next].tag.next;
		entries[next].tag.next= tag;
		entries[tag].tag.previous= next;
	}
	else
	{
		entries[0].header.tags= tag;
	}
	if (*name <= 3 * sig_size)
	{
		int size= 1+*name;
		if (size > 3*sig_size)  size= 3*sig_size; 
		memcpy(entries[tag].tag.name_string, 
		       name(),
		       size);
	}
	else
	{
		entries[entries[tag].tag.name= Alloc_String(name())]
			.string.previous= tag;
	}
	entries[entries[tag].tag.value= Alloc_String(value())]
		.string.previous= tag; 
}

String Book::Get_String(usigf_t i) const
{
	assert (entries[i].flags & bo_string);

	if (!entries[i].string.next)
	{
	  size_t len= strnlen(entries[i].string.text, 5*sig_size);
		return String(entries[i].string.text, len);
	}
	return String(entries[i].string.text, 5*sig_size)
		+ Get_String(entries[i].string.next);
}

bool Book::Merge_Into_Game(Game &game)
{
	if (entries[0].header.starting)
	{
		if (! game.New(Get_String(entries[0].header.starting)))
			return false;
	}
	else
	{
		game.New();
	}

	/* Merge tags */
	Hash_Table tags;
	usigf_t next= entries[0].header.tags;
	while (next)
	{
		String name= Tag_Name(next);
		String value= Tag_Value(next);
		tags.Set(name, value);
#if 0
		String name;
		if (entries[next].tag.name)
			name= Get_String(entries[next].tag.name);
		else
		{
			Object_Reset(String, &name, entries[next].tag.name_string,
				     strnlen(entries[next].tag.name_string, 3*sig_size));
		}
		tags.Set(name, Get_String(entries[next].tag.value));
#endif /* 0 */
		next= entries[next].tag.next;
	}
	game.info.Set(tags);
	
	/* merge moves */
	Go_To_Start();
	Merge_Current_Variation_Into_Game(game);

	return true; 
}

void Book::Merge_Current_Variation_Into_Game(Game &game)
{
	assert (entries[current].flags & bo_position); 

	/* Comment */
	game.Set_Comment(Comment());

	/* NAG */
	if (current != entries[0].header.first)
		game.Set_NAG(entries[current].position.nag);

	/* Subvariations */ 
	usigf_t next= entries[current].position.current;
	while (next)
	{
		move_t small_move= entries[next].move.move;
		int index= game.Current_Generator().Small_Move(small_move);
		assert (index >= 0);
		if (game.Current_Location() !=
		    game.Current_Variation().Length())
			game.Append_Variation();
		game.Append_Move(index); 
		Go_Forward(next);
		Merge_Current_Variation_Into_Game(game);
		Go_Backward();
		game.Go_Backward(); 
		next= entries[next].move.cnext;
	}
}

Book::usigf_t Book::Get_Move(move_t move, bool ignore_detail) const
{
	assert (!ignore_detail || !(move>>16));
	usigf_t next= entries[current].position.current;
	while (next)
	{
		assert (entries[next].flags & bo_move);
		Verbose_Small_Move(entries[next].move.move);
		if (ignore_detail 
		    ? (entries[next].move.move & 0xFFFF) == move
		    : entries[next].move.move == move) 
			return next;
		next= entries[next].move.cnext;
	}
	return 0;
}

String Book::Tag_Name(usigf_t i) const
{
	assert (entries[i].flags & bo_tag);
	if (entries[i].tag.name)
	{
		return Get_String(entries[i].tag.name);
	}
	else
	{
		return String(entries[i].tag.name_string,
			      strnlen(entries[i].tag.name_string,
				      3*sig_size));
	}
}

String Book::Tag_Value(usigf_t i) const
{
	assert (entries[i].flags & bo_tag);
	return Get_String(entries[i].tag.value);
}

String Book::Tag(const char *name) const
{
	usigf_t next= entries[0].header.tags;
	while (next)
	{
		if (Tag_Name(next) == name)
		{
			return Tag_Value(next);
		}
		next= entries[next].tag.next;
	}
	return "";
}

