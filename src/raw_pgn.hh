#ifndef RAW_PGN_HH
#define RAW_PGN_HH

/* An undecoded PGN game  */

#include "string.hh"
#include "hash_table.hh"
#include "vector.hh"
#include "game_info.hh"

class Raw_PGN
{
public:
	void Set_Tags(const Hash_Table &new_tags)        { tags= new_tags;   }
	void Clear_Moves()                               { moves.Free();     }
	void Append_Move_Line(String line)
		{
			if (! moves.Set(*moves + 1)) return;
			moves[-1]= line;
		}
	String Tag(String tag) const
		{
			String result;
			if (! tags.Get(tag, result))  return "";
			return result;
		}
	const Hash_Table &Tags() const               { return tags;      }
	const Vector <String> &Moves() const         { return moves;     }
	void Reset();

	void Normalize(); 

	int Line_Count() const                       { return *moves;    }
	const String &Line(int i) const              { return moves[i];  }

private:
	Hash_Table tags;
	Vector <String> moves;
};

#endif /* ! RAW_PGN_HH */
