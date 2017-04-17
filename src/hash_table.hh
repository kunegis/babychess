#ifndef HASH_TABLE_HH
#define HASH_TABLE_HH

/* This implementation sucks but is enough for the usage in BabyChess. 
 */

#include "string.hh"
#include "vector.hh"

class Hash_Table
{
public:
	bool Get(String key, String &value) const;
	/* Return whether there is such a key.  If yes, then VALUE is
	 * set.  */
	int operator * () const
		{
			assert (*keys == *values);
			return *keys; 
		}
	String Key(int i) const { return keys[i]; }
	String Value(int i) const {return values[i]; }
	void Set(String key,
		 String value);
	void Remove(String key);
	String Get(String key)
		{
			String value;
			return Get(key, value) ? value : ""; 
		}
	void Free()
		{
			keys= values= 0;
		}

private:
	Vector <String>  keys;
	Vector <String>  values;
};

#endif /* ! HASH_TABLE_HH */ 
