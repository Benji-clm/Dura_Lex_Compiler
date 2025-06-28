int
f ()
{
  int arr[4] = { 100, 200, 300, 400 };
  int *ptr = arr;

  ptr++;
  ptr++;

  return *ptr;
}
