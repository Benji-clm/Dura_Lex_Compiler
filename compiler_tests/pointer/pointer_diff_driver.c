#include <stdio.h>

int f ();

int
main ()
{
  int x = f ();
  printf ("x: %d\n", x);
  return !(x == 4);
}
