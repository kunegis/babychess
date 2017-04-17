#ifndef MATERIAL_HH
#define MATERIAL_HH

/* The count of pieces for each side of a position.  The number of
 * kings is also counted.  Counting the king count is easier and more
 * efficient than checking for "pt != pt_K" everytime. 
 */

#include "piece.hh"

#include <assert.h>
#include <string.h>

class Material
{
public:

	int operator () (int cc, int pt) const
		{
			assert (pt >= 0 && pt < 6 &&
				cc >= 0 && cc < 2);
			return count[pt][cc];
		}
	int &operator () (int cc, int pt)
		{
			assert (pt >= 0 && pt < 6 &&
				cc >= 0 && cc < 2);
			return count[pt][cc];
		}
	int operator () (int piece) const
		{
			return count[piece & pi_type][Piece_Color(piece)];
		}
	int &operator () (int piece) 
		{
			return count[piece & pi_type][Piece_Color(piece)];
		}

	void Clear()
		{
			memset(count, '\0', sizeof (count));
		}

	bool operator == (const Material &material) const
		{
			for (int i= 0;  i < 6; ++i)
				for (int j= 0;  j < 2;  ++j)
					if (material.count[i][j] != count[i][j])
						return false;
			return true; 
		}

	void Invert()
		{
			for (int pt= 0;  pt < 6;  ++pt)
			{
				int tmp= count[pt][0];
				count[pt][0]= count[pt][1];
				count[pt][1]= tmp; 
			}
		}

#ifndef NDEBUG
	bool Check() const /* consistency check */
		{
			for (int pt= 0;  pt < 6;  ++pt)
				for (int cc= 0;  cc < 2;  ++cc)
					if (count[pt][cc] < 0)
						return false;
			return true;
		}
#endif /* ! NDEBUG */

private:

	int count[6][2];
};

#endif /* ! MATERIAL_HH */
