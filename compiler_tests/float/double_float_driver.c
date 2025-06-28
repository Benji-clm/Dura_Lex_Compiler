#include <math.h>
#include <stdio.h>

double epsilon = 1e-6;

double f ();

int
main ()
{
  double x = f ();
  printf ("x: %.15f \n", x);
  return !(x == 3.146);
}
