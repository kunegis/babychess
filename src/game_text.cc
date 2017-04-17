#include "game_text.hh"

//#include "san.hh"
#include "format_move_name.hh"
#include "format.hh"
#include "nag.hh"
#include "gettext.hh"

#include <ctype.h>

class Game_Text_Writer
{
public:
	enum {f_image= (1 << 0)};
	virtual int Flags() const= 0;
	virtual void Push()= 0;
	virtual void Pop()= 0;
	virtual void Push_Comment()= 0;
	virtual void Pop_Comment()= 0;
	virtual void Write(const char *word, const char *space= " ")= 0;
	virtual void Write_Image(const Position &)= 0;

	virtual ~Game_Text_Writer() {}
};


static void Game_Text_Write(Game_Text_Writer &, Book &book);
static void Game_Text_Write_Variation(Game_Text_Writer &, Book &);

bool Game_Text::Write_PGN(FILE *file, Book &book)
{
	/* Tags */
	const char *const names[]= 
		{ "Event", "Site", "Date", "Round", "White", "Black",
		  "Result" };
	for (int i= 0;  i < (int) (sizeof (names) / sizeof (names[0])); ++i)
	{
		String value= book.Tag(names[i]);
		fprintf(file,
			"[%s \"%s\"]\n",
			names[i],
			value());
	}
	fprintf(file, "\n");
	
	/* Moves */ 
//	Write(file, book);
	fprintf(file, "\n\n");

	return ferror(file) == 0;
}

static const char *const game_text_html_tags[][2] =
{ { "",   ""},
  { "(",  ")" },
  { "(",  ")" },
  { "[",  "]" },
  { "{",  "}" },
  { "<",  ">" },
  { "«",  "»" }};
enum {max_level= 4};


class HTML_Writer
	:  public Game_Text_Writer
{
public:
	HTML_Writer(FILE *new_file)
		:  file(new_file),
		   level(0),
		   comment_level(0),
		   space("")
		{}
private:
	FILE *file;
	int level; /* subvariation_level */ 
	int comment_level;
	const char *space;
	/* Space to use next if not closing a brace */ 

	int Flags() const {return f_image; }
	void Push()
		{
			
			int l= (int)(sizeof(game_text_html_tags)/
				     sizeof(game_text_html_tags[0])-1);
			if (level < l)  l= level; 
			if (level) fputs("</SPAN>", file);
			int class_level= level;
			if (class_level > max_level)  class_level= max_level; 
			fprintf(file, "\n<SPAN class=\"pgn_level_%d\">%s",
				class_level,
				game_text_html_tags[l][0]);
			++level;
			space= "";
		}
	void Pop()
		{
			assert (level > 0);
			-- level;
			int l= (int)(sizeof(game_text_html_tags)/
				     sizeof(game_text_html_tags[0])-1);
			if (l > level)  l= level; 
			fprintf(file, "%s</SPAN>", game_text_html_tags[l][1]);
			if (level)
			{
				int class_level= level-1;
				if (class_level > max_level)
					class_level= max_level; 
				fprintf(file, 
					"<SPAN class=\"pgn_level_%d\">",
					class_level);
			}
			space= " ";
		}
	void Push_Comment()
		{
			assert (level > 0);
			++ comment_level;
			fprintf(file, "</SPAN><SPAN class=\"pgn_comment\">");
//			space= "";
		}
	void Pop_Comment()
		{
			assert (level > 0);
			assert (comment_level == 1);
			fprintf(file, "</SPAN><SPAN class=\"pgn_level_%d\">",
				level-1);
			-- comment_level; 
			space= " ";
		}
	void Write(const char *text, const char *new_space)
		{
			fputs(space, file);
			fputs(text, file);
			space= new_space; 
		}
	void Write_Image(const Position &p)
		{
			fprintf(file,
				"</SPAN></P>\n<TABLE cellpadding=\"0\" cellspacing=\"0\" "
				"align=\"center\" style=\"border: outset 7pt;\"\n"
				"<CAPTION style=\"caption-side: bottom;\">%s</CAPTION>\n",
				p.To_Move()?_("Black to move"):_("White to move"));
			for (int r= 7;  r >= 0;  --r)
			{
				fprintf(file, "<TR>");
				for (int f= 0;  f < 8;  ++f)
				{
					int piece= p[Make_Square(f, r)];
					fprintf(file, "<TD><IMG src=\"%c%c%c.png\"/></TD>",
						piece&pi_empty?'_':"wb"[Piece_Color(piece)],
						piece&pi_empty?'_':"PNBRQK"[piece&pi_type],
						"bw"[(r+f)%2]);
					
				}
				fprintf(file, "</TR>\n");
			}
			if (comment_level)
				fprintf(file, "</TABLE><P><SPAN class = \"pgn_comment\">");
			else
			fprintf(file, "</TABLE><P><SPAN class=\"pgn_level_%d\">",
				level-1);
		}
};

bool Game_Text::Write_HTML(FILE *file, Book &book)
{
	/* Style */
	fprintf(file,
		"<HTML>\n"
		"  <HEAD>\n"
		"    <META http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n"
		"    <TITLE>Chess game</TITLE>\n"
		"    <LINK rel=\"stylesheet\" type=\"text/css\" href=\"pgn.css\">\n"
		"  </HEAD>\n"
		"  <BODY>\n");

	/* Tags */
	fprintf(file, "<UL>\n");
	Book::usigf_t tag= book[0].header.tags;
	while (tag)
	{
		String name= book.Tag_Name(tag);
		String value= book.Tag_Value(tag);
		tag= book[tag].tag.next;
		fprintf(file, "<LI><B>%s</B>: %s\n",
			name(),
			value());
	}
	fprintf(file, "</UL>\n");

	/* Moves */ 
	HTML_Writer writer(file);
	fprintf(file, "<P>");
	Game_Text_Write(writer, book);
	fprintf(file, "</P>\n");

	fprintf(file, "</BODY></HTML>\n");

	return ferror(file) == 0;
}

void Game_Text_Write(Game_Text_Writer &writer, Book &book)
{
	writer.Push();
	Game_Text_Write_Variation(writer, book);
	writer.Pop();
}

void Game_Text_Write_Variation(Game_Text_Writer &writer, 
			       Book &book)
{
	/* Comment */
	Book::usigf_t comment= book().comment;
	if (comment)
	{
		String text= book.Get_String(comment);
		Verbose("comment= %s", text());
		if (writer.Flags() & Game_Text_Writer::f_image)
		{
			int i= 0;
			while (isspace(text[i])) ++i;
			if (text[i] == '\04')
			{
				writer.Write_Image(book.Get_Position());
				text= text()+i+1;
			}
		}
		if (text)
		{
			writer.Push_Comment();
			writer.Write(text());
			writer.Pop_Comment();
		}
	}
	
	if (book().current == 0)
		return;

	/* Number */
	int n= book.Move_Number();
	if (n%2 == 0 || comment)
	{
		String text= Format("%d%s", n/2+1, n%2?"...":".");
		writer.Write(text(), " ");
		/* The space above is a non-breaking space in UTF-8 */ 
	}
	
	/* The move */ 
	Generator g;
	g.New(book.Get_Position());
	int i= g.Small_Move(book.Get_Move_Entry(book().current).move);
	String name= Format_Move_Name(g.Get_Move_Name(i),
				      san_notation,
				      san_notation_settings);
	name+=NAG(book.Get_Position_Entry(book.Get_Move_Entry(book().current).after).nag);
	writer.Write(name());


	/* Variations */ 
	Book::usigf_t next= book.Get_Move_Entry(book().current).cnext;
	while (next)
	{
		writer.Push();
		String number= Format("%d%s", n/2+1, n%2?"...":".");
		writer.Write(number(), " ");
		int i= g.Small_Move(book.Get_Move_Entry(next).move);
		String name= Format_Move_Name(g.Get_Move_Name(i),
					      san_notation,
					      san_notation_settings);
		name+=NAG(book.Get_Position_Entry(book.Get_Move_Entry(next).after).nag);
		writer.Write(name());
		book.Go_Forward(next);
		Game_Text_Write_Variation(writer, book); 
		book.Go_Backward();
		writer.Pop();
		next= book.Get_Move_Entry(next).cnext;
	}

	/* Rest */ 
	book.Go_Forward();
	Game_Text_Write_Variation(writer, book);
	book.Go_Backward();
}

