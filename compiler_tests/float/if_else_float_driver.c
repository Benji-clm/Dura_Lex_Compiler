#include <stdio.h>

float f ();

int
main ()
{
  float x = f ();
  printf ("result %f \n", x);
  return !(x == 1.2f);
}
