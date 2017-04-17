#ifndef BIT_BOARD_HH
#define BIT_BOARD_HH

#include "inttypes.hh"

class Bit_Board
{
public:
	int Get_Rank(int rank) const
		{
			return (bits >> (rank << 3)) & 0xFF;
		}
	void Set()
		{
			bits= UINT64_C(0xFFFFFFFFFFFFFFFF); 
		}
	void Set(int square)
		{
			bits |= (UINT64_C(1) << (File(square) | (Rank(square) << 3)));
		}
	void Empty() {bits= 0; }
	bool Is() const
		{  return bits != 0; }
	uint64_t Get() const {return bits;}


private:
	uint64_t bits;
};

class Bit_Board_Iterator
{
public:
	Bit_Board_Iterator(const Bit_Board &b)
		:  bits (b.Get()),
		   s(0)
		{
		}
	bool Next(int &file, int &rank) 
		{
			if (bits == 0)
				return false;
			while ((bits & 0xFF) == 0)
			{
				s += 8;
				bits >>= 8;
				assert (s < 256);
			}
			while ((bits & 1) == 0)
			{
				s++;
				bits >>= 1;
				assert (s < 256);
			}
			file= s % 8;
			rank= s / 8;
			bits >>= 1;
			s++;
			return true;
		}
	
	
private:
	uint64_t bits;
	int s;
};

#endif /* ! BIT_BOARD_HH */ 
