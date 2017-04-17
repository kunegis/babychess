#include "dump_book.hh"

#include "strnlen.hh"

#include <stdio.h>

void Dump_Book(const Book &book)
{
	assert (book.Is()); 

	printf("(local format) [%d] %d %d %d %d.\n",
	       book[0].header.size,
	       book[0].header.first,
	       book[0].header.starting,
	       book[0].header.tags,
	       book[0].header.firstn);

	for (Book::usig_t i= 1;  i < book[0].header.size;  ++i)
	{
		const Book::Entry &entry= book[i];
		switch (entry.flags & bo_type)
		{
		default:
			printf("%u ??? [%u]\n", i, entry.flags);
			break;
		case bo_position:
			printf("%u P %u <-> %u  {%u}  %d $%u %d %d\n",
			       i,
			       entry.position.last,
			       entry.position.current,
			       entry.position.comment,
			       entry.position.score,
			       entry.position.nag,
			       entry.position.ep,
			       entry.position.halfmove_clock);
			break;
		case bo_move:
			printf("%u M %u => %u <-> %u,   %u <-> %u <= %u  %c%c-%c%c %c\n",
			       i,
			       entry.move.before,
			       entry.move.cprevious,
			       entry.move.cnext,
			       entry.move.lnext,
			       entry.move.lprevious,
			       entry.move.after,
			       'a' + File(entry.move.move & 0xFF),
			       '1' + Rank(entry.move.move & 0xFF),
			       'a' + File((entry.move.move >> 8) & 0xFF),
			       '1' + Rank((entry.move.move >> 8) & 0xFF),
			       ((entry.move.move >> 24) & 0xF) & pi_empty ? ' ' 
			       : "NBRQ"[((entry.move.move >> 24) & 7) - 1]);
			break;
		case bo_tag:
			printf("%u T %u <-> %u  [%u]= %u  \"%s\"\n",
			       i,
			       entry.tag.previous,
			       entry.tag.next,
			       entry.tag.name,
			       entry.tag.value,
			       entry.tag.name_string);
			break;
		case bo_string:
			static char t[5*Book::sig_size], tt='\0';
			tt=tt;
			printf("%u S %u <-> %u \"%s\"\n",
			       i,
			       entry.string.previous,
			       entry.string.next,
			       strnlen(entry.string.text, 5*Book::sig_size) == 5*Book::sig_size
			       ? (memcpy(t, entry.string.text, 5*Book::sig_size), t) 
			       : entry.string.text);
			break;
		}
	}

}
