
#include <string.h>
int main()
{
   char *p= strdupa("Hello");
   return strlen(p) - 5;
}
