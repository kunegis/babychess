#include <stdio.h>
#include <string.h>

int main()
{
	char buf[100];
	
	int len= sprintf(buf, "%'d", 1234567);
	/* If ' works, some separator chars are inserted and the length will be
	 * more than 7. 
	 */

	printf("len= %u, buflen= %u, buf=\"%s\"\n", 
	       len,
	       strlen(buf),
	       buf);
	
	if (len != strlen(buf))   return 2;

	return len >= 7 ? 0 : 1;
}
