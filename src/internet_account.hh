#ifndef INTERNET_ACCOUNT_HH
#define INTERNET_ACCOUNT_HH

#include "pointer.hh"
#include "string.hh"

class Internet_Account
	:  public Pointed
{
public:

	String host;
	int port;
	String user, password;

	void Reset()
		{
			host= "freechess.org";
			port= 5000;
			user= "guest";
			password= "";
		}
};

#endif /* ! INTERNET_ACCOUNT_HH */
