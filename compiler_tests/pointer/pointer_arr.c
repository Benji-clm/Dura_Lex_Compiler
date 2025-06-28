int
f ()
{
  int arr[5] = { 10, 20, 30, 40, 50 };
  int *ptr = arr;

  ptr += 2;
  ptr -= 1;

  return *ptr;
}
