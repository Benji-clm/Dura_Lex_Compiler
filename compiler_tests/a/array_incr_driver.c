#include <stdio.h>

int f ();

int
main ()
{
  int result = f ();
  printf ("f() returned: %d\n", result);
  ;
  return !(result == 15);
}
