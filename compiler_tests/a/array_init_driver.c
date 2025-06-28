#include <stdio.h>

int f ();

int
main ()
{
  printf ("Example function returned: %d\n", f ());
  return !(f () == 4);
}
