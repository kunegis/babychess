#include "notation.hh"

#include "verbose.hh"
#include "format.hh"
#include "install_dir.hh"
#include "gettext.hh"
#include "stream_file.hh"
#include "filename.hh"
#include "magic.hh"
#include "read_line.hh"

/* S= SAN, F= french, D= german */
const int san_chars[26]=
/**/{8,2,1,4,8,2,8,8,8,8,5,2,8,1,8,0,4,3,1,3,8,8,8,8,8,8};
/*   A B C D E F G H I J K L M N O P Q R S T U V W X Y Z       (letter)
 *     S F D   F         S     S   S S S D F                   (language)
 */

Notation::Notation(const char *const *s)
{
	int i= i_count;
	while (i)
	{
		assert (*s);
		texts[i_count-i]= *s;
		++s;
		--i;
	}
	assert (!*s);
}

void Notation::Reset()
{
	const char *text= Default_Conf_Name(); 
	Verbose_Str(text); 
	String filename= Install_Dir()/"share/notation"/text;
	if (! Read_From_File(filename(), *this))
	{
		*this= san_notation;
	}
}

const char *Notation::Default_Conf_Name()
{
	/* The name of the file in share/notation that is
	 * used by default.  */
	return I_("local notation name:English"); 
}

bool Read_Stream(FILE *file, Notation *notation)
{
	if (! Magic::Read(file, "Notation"))  return false;

	int i= 0;
	for (; i < i_count;  ++i)
	{
		if (! Read_Line(file, notation->texts[i]))  break;
	}

	/* Make appending strings backward compatible */ 
	for (; i < i_count;  ++i)  notation->texts[i]= san_notation.texts[i];

	return true;
}


bool Write_Stream(FILE *file,  const Notation *notation)
{
	if (! Magic::Write(file, "Notation"))  return false;

//	if (0 > fprintf(file, "%d\n", notation->options))  return false;
	
	for (int i= 0;  i < i_count;  ++i)
		if (0 > fprintf(file, "%s\n", notation->texts[i]()))  return false;
	return true; 
}

bool Read_Stream(FILE *file, Notation_Settings *settings)
{
	if (2 != fscanf(file, "%u %u", &settings->type, &settings->flags))
		return false;
	if (settings->type >= n_count ||
	    settings->flags >= (1 << nos_count))
	{
		errno= EINVAL;
		return false;
	}
	return true;
}

bool Write_Stream(FILE *file, const Notation_Settings *settings)
{
	assert (settings->type >= 0);
	assert (settings->type < n_count);
	assert (settings->flags >= 0);
	assert (settings->flags < (1 << nos_count));
	fprintf(file, "%d %d\n", settings->type, settings->flags);
	return ! ferror(file);
}

const char *const san_text[i_count+1]= 
{
	"e.p.", "O-O", "O-O-O", "P", "N", "B", "R", "Q", "K", "x", "=",
	"w", "b", "+", "abcdefgh", NULL
};

Notation san_notation(san_text);

Notation_Settings san_notation_settings
(n_short, 
 nos_no_ep|nos_no_double_check);
