#include "book_scores.hh"

int Set_Position_Score(Book &book, Book::usigf_t position)
{
	if (book[position].position.score == ~(Book::sig_t)0)
		return 0;
	book.Set_Score(position, ~(Book::sig_t)0);
	int count= 0;
	Book::sigf_t next= book[position].position.current;
	while (next)
	{
		count += Set_Position_Score(book, book[next].move.after);
		count += 1;
		next= book[next].move.cnext;
	}
	assert (count >= 0);
	book.Set_Score(position, count);
	return count;
}

void Set_Book_Scores(Book &book)
{
	Set_Position_Score(book, book[0].header.first);
}
