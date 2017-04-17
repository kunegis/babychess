#include "variation.hh"

void Variation::Free()
{
	moves.Free();
	names.Free();
	comments.Free();
	variations.Free();
}

bool Variation::
Append_Move(const Generator &generator,
	    int move_index)
{
	if (! moves.Set(*moves + 1))
		return false;
	
	if (! names.Set(*moves))
	{
		moves= -1;
		return false;
	}

	moves[-1]= generator[move_index];
	names[-1]= generator.Get_Move_Name(move_index);

	return true;
}

void Variation::Cut(int new_length)
{
	assert (new_length >= 0   &&
		new_length <= *moves);
	if (new_length == *moves)
		return;
	moves= new_length;
	names= new_length;
	if (new_length < *comments)
		comments= new_length;
	if (new_length < *variations)
		variations= new_length; 
}

bool Variation::
Append_Variation(int location)
{
	assert (location >= 0);
	assert (location < *moves);

	if (*variations <= location)
	{
		if (! variations.Set(location + 1))
			return false;
	}

	if (! variations[location].Set(*variations[location] + 1))
		return false;

	if (! variations[location][-1].Alloc(previous_move_count + location))
	{
		variations[location]= -1;
		return false;
	}

	Check_Variations(); 

	return true; 
}

void Variation::
Set_Comment(int i, String comment)
{
	assert(i >= 0);
	assert(i <= *moves);

	if (*comments <= i)
	{
		if (! comment.Is())
			return; 
		if (! comments.Set(i + 1))
			return;
	}

	comments[i]= comment; 
}

void Variation::Remove_Variation(int location,
				 int variation_index)
{
	variations[location][variation_index].Free(); 
	if (variation_index + 1 < *variations[location])
		variations[location][variation_index] << variations[location][-1];
	variations[location]= -1;
	Check_Variations();
}

void Variation::Set_NAG(int location, 
			int nag)
{
	assert (location >= 0 && location < *moves);

	if (location >= *nags)
	{
		if (nag == 0)
			return; 
		int osize= *nags;
		if (! nags.Set(location + 1))
			return;
		memset(nags(osize), '\0', location - osize); 
	}

	nags[location]= nag; 
}

bool Variation::Append_Moves(Variation &variation)
{
	Check_Variations(); 
	variation.Check_Variations(); 
	int size= *moves;
	moves.Append_Transfer_From(variation.moves);
	names.Append_Transfer_From(variation.names);
	if (*variation.nags)
	{
		int osize= *nags;
		nags= size;
		memset(nags(osize), '\0', size - osize);
		nags.Append_Transfer_From(variation.nags);
	}
	if (*variation.comments)
	{
		comments= size + 1;
		/* First comment is lost.  Normalized subvariations
		 * have no first comment. 
		 */
		comments.Append_Transfer_From(variation.comments, 1);
	}
	if (*variation.variations)
	{
		variations= size;
		variations.Append_Transfer_From(variation.variations);
	}
	Check_Variations();
	variation.Check_Variations(); 
	return true; 
}

bool Variation::
Append_Variation(int location,
		 Dynamic <Variation> &variation)
{
	assert (variation.Is());
	variation->Check_Variations();
	Check_Variations(); 
	if (*variations <= location)
	{
		if (! variations.Set(location + 1))
			return false;
	}
	if (! variations[location].Set(*variations[location] + 1))
		return false;
	variations[location][-1] << variation;
	Check_Variations();
	return true; 
}

Variation::Variation(Variation &variation,
		     int location)
{
	assert (location >= 0); 
	variation.Check_Variations();

	previous_move_count= variation.previous_move_count + location;

	int size= *variation - location;
	assert (size >= 0);

	moves.Append_Transfer_From(variation.moves, location);
	names.Append_Transfer_From(variation.names, location);
	
	if (* variation.nags > location)
		nags.Append_Transfer_From(variation.nags, location);
	if (* variation.comments > location)
		comments.Append_Transfer_From(variation.comments, location);
	if (* variation.variations > location)
		variations.Append_Transfer_From(variation.variations, location);
	Check_Variations(); 
	variation.Check_Variations(); 
}

void Variation::Normalize()
{
	for (int i= 0;  i < *moves;  ++i)
	{
		if (i < *variations)
		{
			for (int j= 0;  j < *variations[i];  ++j)
			{
				if (!*variations[i][j]())
				{
					variations[i][j] << variations[i][-1];
					variations[i]= -1;
					-- j;
				}
				else
				{
					Normalize_Subvariation(i, j);
				}
			}
		}
	}
	Check_Variations(); 
}

void Variation::
Normalize_Subvariation(int i,
		       int j)
{
	/* reparent variations at index 0 */ 
	if (*variations[i][j]().variations)
	{
		int count= *variations[i][j]().variations[0];
		if (count)
		{
			if (variations[i].Set(*variations[i] + count))
			{
				for (int k= 0;  k < count;  ++k)
				{
					variations[i][k - count]
						<<variations[i][j]().
						variations[0][k];
				}
				variations[i][j]().variations[0]= 0;
			}
		}
	}

	/* remove comment at index 0 */
	String comment= variations[i][j]().Get_Comment(0);
	if (comment.Is())
	{
		Append_Comment(i, comment);
		variations[i][j]().Set_Comment(0, "");
	}

	/* normalize subvariation */ 
	variations[i][j]().Normalize();
}

void Variation::Append_Comment(int location, String comment)
{
	Set_Comment(location, Get_Comment(location) + comment); 
}

#ifndef NDEBUG
void Variation::Check_Variations() const
{
	for (int i= 0;  i < *variations;  ++i)
	{
		if (!*variations[i])  continue;
		for (int j= 0;  j < *variations[i];  ++j)
		{
			assert (variations[i][j].Is());
			variations[i][j]->Check_Variations(); 
		}
	}
}
#endif /* ! NDEBUG */ 

	String Variation::Get_Comment(int index) const
		{
			if (index < *comments)
				return comments[index];
			else
				return ""; 
		}
