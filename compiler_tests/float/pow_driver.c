#include <stdio.h>

float f (float x, int n);

int
main ()
{
  float result = f (5.0f, 3);
  printf ("Function returned: %f\n", result);
  return !(result == 125.0f);
}
