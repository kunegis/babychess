#include "internet_account_stream.hh"

bool Read_Stream(FILE * file,
		 Internet_Account *internet_account)
{
	char host[32 + 1];
	char user[32 + 1];
	char password[32 + 1];

	int port;
	
	int sf= fscanf(file,
		       "%32[a-zA-Z0-9_-] @ %32[a-z.] : %d : %32s",
		       user,
		       host,
		       & port,
		       password);

	if (sf != 4)
	  return false;

	internet_account->host= host;
	internet_account->user= user;
	internet_account->password= password;
	internet_account->port= port;

	return true;
}

bool Write_Stream(FILE *file,
		  const Internet_Account *internet_account)
{
	return 0 <= fprintf(file,
			    "%s@%s:%d:%s\n",
			    internet_account->user(),
			    internet_account->host(),
			    internet_account->port,
			    internet_account->password());
}
