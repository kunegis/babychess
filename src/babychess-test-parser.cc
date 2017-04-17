#include "main.hh"
#include "parser.hh"

int Main(int, char **)
{
	Parser parser("767676:-45j");

	unsigned a;
	int b;
	
	if (2 != parser.Next_Format("%u:%d", &a, &b))  
		return 1;
	if (a != 767676)   return 1;
	if (b != -45)      return 1;

	if (2 != parser.Skip_Format("%u:%d", &a, &b))  
		return 1;
	if (a != 767676)   return 1;
	if (b != -45)      return 1;

	if (parser.Next_Char() != 'j')   
		return 1;

	return 0;
}
